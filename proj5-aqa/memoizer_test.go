package memoizer

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	"os"
	"testing"
	"time"

	"github.com/61c-teach/sp18-proj5"
	"github.com/petar/GoMNIST"
)

/* How much buffering to use */
const bufSize = 100

/* Input Data */
const trainDataPath = "./data/train-images-idx3-ubyte.gz"
const trainLblPath = "./data/train-labels-idx1-ubyte.gz"

/* Initializes the whole system using the provided functions for the various services.
Returns: a handle to the memoizer */
func startup(memFunc func(proj5.MnistHandle, proj5.MnistHandle, proj5.CacheHandle),
	classFunc func(proj5.MnistHandle),
	cacheFunc func(proj5.CacheHandle),
) proj5.MnistHandle {
	/* Create all the services that we're going to use and their handles. Note
	that a more sophisticated test might use stand-in's for these services that
	intentionally behave badly in order to test error conditions that might be
	rare in the real implementation. */
	classHandle := proj5.MnistHandle{
		make(chan proj5.MnistReq, bufSize),
		make(chan proj5.MnistResp, bufSize),
	}
	go classFunc(classHandle)

	cacheHandle := proj5.CacheHandle{
		make(chan proj5.CacheReq, bufSize),
		make(chan proj5.CacheResp, bufSize),
	}
	go cacheFunc(cacheHandle)

	memHandle := proj5.MnistHandle{
		make(chan proj5.MnistReq, bufSize),
		make(chan proj5.MnistResp, bufSize),
	}
	go memFunc(memHandle, classHandle, cacheHandle)

	return memHandle
}

// Minimal correctness testing
func TestMemoizerMinimal(t *testing.T) {
	rawTrain, err := GoMNIST.ReadSet(trainDataPath, trainLblPath)
	if err != nil {
		panic(fmt.Sprintf("Failed to load training set from %s and %s: %v\n",
			trainDataPath, trainLblPath, err))
	}

	// Used to ensure that message IDs are always globally unique
	var reqID int64 = 0

	/* If you're curious what these images look like, you can uncomment this */
	// Show(rawTrain.Images[0])

	// Initialize our system for basic testing
	memHandle := startup(Memoizer, proj5.MnistServer, proj5.Cache)

	/* Note for the following tests that we are using the CheckImage function
	that was defined in proj5/testing_helpers.go. Using the same unit tests as
	the classifier helps us make sure that we really are implementing the exact
	same interface. But don't get too cocky, there's no guarantee that the
	proj5 tests are perfect! */

	// == Basic classification test ==
	// Keep in mind, not all inputs must give the right label because this
	// classifier isn't perfect. In fact, offline tests show that it's only about
	// 86% accurate. I've verified manually that the classifier gets this one
	// right though. If you change the classifier, and this starts failing, check
	// to make sure it's at least reasonable.
	proj5.CheckImage(rawTrain.Images[0], int(rawTrain.Labels[0]), memHandle, &reqID, t)

	// == Make sure it works the same way twice in a row == */
	proj5.CheckImage(rawTrain.Images[0], int(rawTrain.Labels[0]), memHandle, &reqID, t)

	//== Test some more values, make sure the model is deterministic.
	// These tests should always pass, no matter the model since they only check
	// that the response is reasonable
	firstResp := proj5.CheckImage(rawTrain.Images[1], -1, memHandle, &reqID, t)
	if resp := proj5.CheckImage(rawTrain.Images[1], -1, memHandle, &reqID, t); resp != firstResp {
		t.Errorf("Classification on second attempt doesnt match first, %d != %d", firstResp, resp)
	}

	// == Close the channel (this must be the last test!)
	close(memHandle.ReqQ)
}

/* This is how much faster the hot cache run should at least be. */
const cacheSpeedup = 2.0

// Tests if the memoizer is really memoizing (i.e. using the cache)
func TestMemoizerSpeedup(t *testing.T) {
	rawTrain, err := GoMNIST.ReadSet(trainDataPath, trainLblPath)
	if err != nil {
		panic(fmt.Sprintf("Failed to load training set from %s and %s: %v\n",
			trainDataPath, trainLblPath, err))
	}

	memHandle := startup(Memoizer, proj5.MnistServer, proj5.Cache)

	//Globally unique request ID
	var reqID int64 = 0

	// == Test a bunch of images that we've never seen before (cache can't help us)
	start := time.Now()
	proj5.CheckImages(rawTrain.Images[2:], nil, memHandle, &reqID, t)
	coldRunTime := time.Since(start)

	// == Now test again with the same images, the cache should help here
	start = time.Now()
	proj5.CheckImages(rawTrain.Images[2:], nil, memHandle, &reqID, t)
	hotRunTime := time.Since(start)

	// == The second run should be faster
	if coldRunTime/hotRunTime < cacheSpeedup {
		t.Errorf("The cache didn't seem to help. Cold run: %v, Hot run: %v", coldRunTime, hotRunTime)
	}
	fmt.Printf("Cold run took %v, Hot run took %v\n", coldRunTime, hotRunTime)

	// == Close the channel (this must be the last test!)
	close(memHandle.ReqQ)
}

/* Will print the raw image to the current directory as "out.png" */
func Show(data GoMNIST.RawImage) {
	const dim = 28
	/* Create an empty image */
	m := image.NewNRGBA(image.Rect(0, 0, dim, dim))

	/* Copy the bytes into it (in grayscale) from the raw image */
	for y := 0; y < dim; y++ {
		for x := 0; x < dim; x++ {
			v := data[y*dim+x]
			m.Set(x, y, color.RGBA{v, v, v, 255})
		}
	}

	/* Write the image to a file. Note the use of "defer", this runs the provided
	function after this function returns, but before the caller resumes. It's
	kind of like a finally clause on a try/catch. */
	f, _ := os.OpenFile("out.png", os.O_WRONLY|os.O_CREATE, 0600)
	defer f.Close()
	png.Encode(f, m)
}
