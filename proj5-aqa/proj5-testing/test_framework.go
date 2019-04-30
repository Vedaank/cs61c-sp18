package memoizer_testing

import (
	"testing"
	"time"

	//memoizer "github.com/61c-teach/proj5-impls/memoizer_good"
	memoizer "bitbucket.org/vedaank61c/proj5-aqa"

	proj5 "github.com/61c-teach/sp18-proj5"
	"github.com/petar/GoMNIST"
)

// The longest any checker is allowed to run before we give up on it (in seconds)
const checkerTimeout = 5

/* How much buffering to use */
const bufSize = 100

/* Input Data */
const trainDataPath = "../data/train-images-idx3-ubyte.gz"
const trainLblPath = "../data/train-labels-idx1-ubyte.gz"

/* This test makes heavy use of "mocking" which is when you create a fake
* implementation of something (called a mock implementation) that exhibits some
* behavior. Most of the mocks in this case exhibit "bad" behaviors, like
* crashing, returning errors, or reporting wrong message IDs. This is to ensure
* that our implementation can handle all those cases. */
type mockClass func(proj5.MnistHandle, *testing.T)
type mockCache func(proj5.CacheHandle, *testing.T)
type mockChecker func(handle proj5.MnistHandle, ims []GoMNIST.RawImage, t *testing.T)

func runMockTest(classFunc mockClass, cacheFunc mockCache, checker mockChecker, ims []GoMNIST.RawImage, t *testing.T) {
	// Boot up using the mocks
	classHandle := proj5.MnistHandle{
		make(chan proj5.MnistReq, bufSize),
		make(chan proj5.MnistResp, bufSize),
	}
	go classFunc(classHandle, t)

	cacheHandle := proj5.CacheHandle{
		make(chan proj5.CacheReq, bufSize),
		make(chan proj5.CacheResp, bufSize),
	}
	go cacheFunc(cacheHandle, t)

	memHandle := proj5.MnistHandle{
		make(chan proj5.MnistReq, bufSize),
		make(chan proj5.MnistResp, bufSize),
	}
	go memoizer.Memoizer(memHandle, classHandle, cacheHandle)

	// Run the checker with a timeout (in case the memoizer hangs)
	finCh := make(chan bool)

	// Make sure the code works synchronously when requested (we should get an
	// answer if we only ask for one thing). This is to catch errors in the
	// batching algorithm.
	go func() {
		checker(memHandle, ims, t)
		finCh <- true
	}()
	select {
	case <-finCh:
	case <-time.After(checkerTimeout * time.Second):
		t.Error("Checking one image timed out (memoizer hung?)")
		// Testing can't continue past this (since we close channels and stuff later which could panic)
		t.FailNow()
	}

	//Close the memoizer's channel, block until they close their respQ.
	close(memHandle.ReqQ)

	//There should be nothing pending in the mem respQ (CheckImages should have
	//fully drained it) and the memoizer should close the channel when it's
	//reqQ is closed. We assume that if you close the channel, you also exit.
	// This is implemented with a timeout to prvent hanging if Memoizer doesn't
	// close its channel.
	// XXX I don't currently verify that the memoizer goroutine actually exits,
	// this should be possible from the runtime/pprof api.
	select {
	case resp, ok := <-memHandle.RespQ:
		if ok {
			// Memoizer had extra stuff in the channel
			t.Errorf("Memoizer had extra messages left in the respQ (msgID: %d)", resp.Id)
		}
	case <-time.After(1 * time.Second):
		t.Error("Memoizer didn't close its respQ (and exit) after we closed its reqQ\n")
	}

	// Make sure the memoizer closed the classifier
	select {
	case resp, ok := <-classHandle.RespQ:
		if ok {
			// Memoizer had extra stuff in the channel
			t.Errorf("Memoizer sent requests to classifier after we closed it (msgID: %d)", resp.Id)
		}
	case <-time.After(1 * time.Second):
		t.Error("Memoizer didn't close classifier after we closed its reqQ\n")
	}

	select {
	case resp, ok := <-cacheHandle.RespQ:
		if ok {
			// Memoizer had extra stuff in the channel
			t.Errorf("Memoizer sent requests to cache after we closed it (msgID: %d)", resp.Id)
		}
	case <-time.After(1 * time.Second):
		t.Error("Memoizer didn't close cache after we closed its reqQ\n")
	}
}
