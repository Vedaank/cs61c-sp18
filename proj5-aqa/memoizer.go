package memoizer

import (
	"github.com/61c-teach/sp18-proj5"
	"hash/crc64"
)

/* The simplest possible implementation that does anything interesting.
This doesn't even do memoization, it just proxies requests between the client
and the classifier. You will need to improve this to use the cache effectively. */
func Memoizer(memHandle proj5.MnistHandle, classHandle proj5.MnistHandle, cacheHandle proj5.CacheHandle) {

	crc64Table := crc64.MakeTable(crc64.ECMA)
	var reqID int64 = 0

	defer close(memHandle.RespQ)
	defer close(classHandle.RespQ)
	defer close(cacheHandle.RespQ)

	for req := range memHandle.ReqQ {
		key := crc64.Checksum(req.Val, crc64Table)
		cacheHandle.ReqQ <- proj5.CacheReq{false, key, 0, reqID}
		cacheResp := <- cacheHandle.RespQ

		if cacheResp.Exists {
			if req.Id == cacheResp.Id {
				var Error error
				memHandle.RespQ <- proj5.MnistResp{cacheResp.Val, cacheResp.Id, Error}
			}
		} else {
			classHandle.ReqQ <- req
			classResp := <- classHandle.RespQ
			if classResp.Id == cacheResp.Id {
				memHandle.RespQ <- classResp
				cacheHandle.ReqQ <- proj5.CacheReq{true, key, classResp.Val, reqID}
			} else {
				err := proj5.CreateMemErr(proj5.MemErr_serCorrupt, "Classifier Crashed", nil)
				memHandle.RespQ <- proj5.MnistResp{cacheResp.Val, classResp.Id, err}
			}
		}
		reqID++
	}
}