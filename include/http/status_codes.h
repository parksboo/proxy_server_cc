
#ifndef NSHTTP_STATUS_CODES_H_
#define NSHTTP_STATUS_CODES_H_

namespace proxy_http {

	enum class StatusCode {
		OK = 200,
		BAD_REQUEST = 400,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
	};

}  // namespace ns_http

#endif  // NSHTTP_STATUS_CODES_H_
