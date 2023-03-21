#pragma once

#include <crow/http_request.h>
#include <crow/http_response.h>

namespace insite::arbor {

crow::response GetSpikes(const crow::request &req);

} // namespace insite::arbor
