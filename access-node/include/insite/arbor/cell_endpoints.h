#pragma once
#include <crow/http_request.h>
#include <crow/http_response.h>

namespace insite::arbor {

crow::response GetCellInfos(const crow::request &req);
crow::response GetCells(const crow::request &req);

} // namespace insite::arbor
