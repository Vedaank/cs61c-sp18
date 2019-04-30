package memoizer_testing

import (
	"fmt"
	"testing"

	"github.com/61c-teach/sp18-proj5"
	"github.com/petar/GoMNIST"
)

// Some of the mocks use this fake classifier that is simple, but consistent.
// These tests aren't concerned with real labels, but need to have predictible
// and consistent labels for testing purposes
func lblIm(im GoMNIST.RawImage) int {
	/* The label is just the first byte mod 10 */
	return int(im[0]) % 10
}

//For mocks that are supposed to fail, after which request should they fail?
const whenFail = 10

// Minimal correctness testing
func TestMocks(t *testing.T) {
	/* Read some images using the GoMNIST package. All the data should be
	* available in trainDataPath and trainLblPath which are defined in
	* memoizer.go. Note that in this test, we don't use the lbl data, but the
	* API requires us to load it anyway. Also note that the label data is the
	* 'true' labels, the classifier may not always give the same label and
	* that's OK (machine-learning is only approximate). You probably shouldn't
	* use the label data for anything other than curiosity. */
	rawTrain, err := GoMNIST.ReadSet(trainDataPath, trainLblPath)
	if err != nil {
		panic(fmt.Sprintf("Failed to load training set from %s and %s: %v\n",
			trainDataPath, trainLblPath, err))
	}

	/* This is very similar to the "Correct Labels Real" test, but uses the
	* mock classifier and cache instead. */
	t.Run("MockGood", func(t *testing.T) {
		runMockTest(mockClassifierGood, mockCacheGood, checkFullMock, rawTrain.Images, t)
	})

	t.Run("ClassBadID", func(t *testing.T) {
		runMockTest(mockClassifierBadID, mockCacheGood, checkClassBadId, rawTrain.Images, t)
	})
}

// Well-behaved classifier, doesn't do anything unusual. Uses the lblIm
// function to label images.
func mockClassifierGood(handle proj5.MnistHandle, t *testing.T) {
	defer close(handle.RespQ)

	// Used to ensure that memoizer doesn't mess up IDs some how
	// I'm using the empty struct (struct{}) just as a place holder, we'll only
	// ever test for existence in the map (it's more like a set than a map)
	seenIds := make(map[int64]struct{})

	for req := range handle.ReqQ {
		// Use the first byte of the image % 10 as the label (keep in mind none of
		// our tests actually check that the classifier is actually accurate, just
		// that it's consistent).
		lbl := lblIm(req.Val)

		if _, found := seenIds[req.Id]; found == true {
			t.Errorf("Repeated ID: %d", req.Id)
		} else {
			seenIds[req.Id] = struct{}{}
		}

		handle.RespQ <- proj5.MnistResp{lbl, req.Id, nil}
	}
}

// Well-behaved mock cache. Uses the real cache behind the scenes but checks a
// few more error conditions and lets you mess with it more.
func mockCacheGood(handle proj5.CacheHandle, t *testing.T) {
	// Create a real cache to use behind the scenes
	realHandle := proj5.CacheHandle{
		make(chan proj5.CacheReq, bufSize),
		make(chan proj5.CacheResp, bufSize),
	}
	go proj5.Cache(realHandle)
	defer close(realHandle.ReqQ)
	defer close(handle.RespQ)

	// Used to ensure that memoizer doesn't mess up IDs some how
	// I'm using the empty struct (struct{}) just as a place holder, we'll only
	// ever test for existence in the map (it's more like a set than a map)
	seenIds := make(map[int64]struct{})

	for req := range handle.ReqQ {
		// Check for duplicate IDs (only for reads)
		if req.Write == false {
			if _, found := seenIds[req.Id]; found == true {
				t.Errorf("Repeated ID: %d", req.Id)
			} else {
				seenIds[req.Id] = struct{}{}
			}
		}

		// Then just proxy to the real cache (for reads)
		realHandle.ReqQ <- req
		if req.Write == false {
			resp := <-realHandle.RespQ
			handle.RespQ <- resp
		}
	}
}

// This classifier gives a bad message ID for the whenFail'd request
func mockClassifierBadID(handle proj5.MnistHandle, t *testing.T) {
	defer close(handle.RespQ)

	// Used to ensure that memoizer doesn't mess up IDs some how
	// I'm using the empty struct (struct{}) just as a place holder, we'll only
	// ever test for existence in the map (it's more like a set than a map)
	seenIds := make(map[int64]struct{})

	reqCount := 0
	for req := range handle.ReqQ {
		// Use the first byte of the image % 10 as the label (keep in mind none of
		// our tests actually check that the classifier is actually accurate, just
		// that it's consistent).
		lbl := lblIm(req.Val)

		// This checks to see if the memoizer gave us repeated IDs
		if _, found := seenIds[req.Id]; found == true {
			t.Errorf("Repeated ID: %d", req.Id)
		} else {
			seenIds[req.Id] = struct{}{}
		}

		reqCount++
		if reqCount == whenFail {
			handle.RespQ <- proj5.MnistResp{lbl, req.Id + int64(42), nil}
		} else {
			handle.RespQ <- proj5.MnistResp{lbl, req.Id, nil}
		}
	}
}

// Tests only one image synchronously
// This checker assumes the mock will behave exactly the same as the mock classifier (i.e.
// it will provide the same labels as the mock). It assumes the classifier uses
// lblIm to create labels (you must use a mock, not the real classifier).
func checkOneMock(handle proj5.MnistHandle, ims []GoMNIST.RawImage, t *testing.T) {
	var reqID int64 = 0
	// Because we're using a mock classifier, we know the expected labels, we
	// don't need to do the same trick as checkLabelsReal
	proj5.CheckImage(ims[0], lblIm(ims[0]), handle, &reqID, t)
}

// Tests all images in ims asynchronously (may send multiple requests before waiting for a reply)
// Assumes that you are using the mock classifier that uses lblIms to label
// images (not the real classifier). See the comments in checkOneCorrect for
// more details.
func checkFullMock(handle proj5.MnistHandle, ims []GoMNIST.RawImage, t *testing.T) {
	// The expected labels
	exp := make([]int, len(ims))
	for i, im := range ims {
		exp[i] = lblIm(im)
	}

	var reqID int64 = 0
	proj5.CheckImages(ims, exp, handle, &reqID, t)
}

// Checks if the memoizer correctly forwards a bad ID when the classifier gives one
func checkClassBadId(handle proj5.MnistHandle, ims []GoMNIST.RawImage, t *testing.T) {
	var reqID int64 = 0

	// Pre-compute the expected value for the first whenFail values
	exp := make([]int, whenFail*2)
	for i, im := range ims[:whenFail*2] {
		exp[i] = lblIm(im)
	}

	// The first whenFail-1 misses should work as normal
	proj5.CheckImages(ims[:whenFail-1], exp, handle, &reqID, t)

	// The whenFail'th miss should have an error
	handle.ReqQ <- proj5.MnistReq{ims[whenFail], reqID}
	resp, ok := <-handle.RespQ
	if !ok {
		t.Error("Memoizer exited after classifier gave bad ID.")
	}

	if resp.Err == nil {
		t.Error("Memoizer didn't report an error when classifier gave a bad ID")
		t.FailNow()
	}

	cause := proj5.GetErrCause(resp.Err)
	if cause != proj5.MemErr_serCorrupt {
		t.Errorf("Memoizer returned incorrect error cause. Expected MemErr_serCorrupt, got %v", cause)
	}
	// Note that the ID of this resp is allowed to be bad (although it shouldn't be if you can avoid it)
	reqID++

	// Retry the request (should succeed)
	proj5.CheckImage(ims[whenFail], exp[whenFail], handle, &reqID, t)
}
