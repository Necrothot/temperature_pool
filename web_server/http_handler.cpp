/*
 * Notes:
 *
 *  Make INDEX.HTML in lowercase:
 *      Project -> Properties -> C/C++ Build -> Settings -> Tool Settings -> NetBurner Comphtml
 *          Command: comphtml -dindex.html
 */

#include "http_handler.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <iosys.h>
#include <tcp.h>
#include <http.h>
#include <netinterface.h>
#include <utils.h>
#include <multipartpost.h>
#include <StreamUpdate.h>
#include <bsp.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include "3rdparty/rapidjson/rapidjson.h"
#include "3rdparty/rapidjson/document.h"
#include "3rdparty/rapidjson/stringbuffer.h"
#include "3rdparty/rapidjson/writer.h"

#include "global_definitions/constants.h"
#include "global_definitions/variables.h"
#include "system/temperature_pool.h"

extern const char *PlatformName;

static http_gethandler *oldGetHandler;

int myDoPost(int sock, char *url, char *pData, char *rxBuffer);
int myDoGet(int sock, PSTR url, PSTR rxBuffer);

void registerPost()
{
    if (EnableMultiPartForms(10000000))
    {
        SetNewPostHandler(myDoPost);
    }
}

void registerGet()
{
    oldGetHandler = SetNewGetHandler(myDoGet);
}

/**
 * Use this instead of SendHTMLHeader() when sending response with JSON payload
 * so that client won't have to parse incoming data to JSON object
 */
void sendJSONHeader(int sock)
{
    writestring(sock,
            "HTTP/1.0 200 OK\r\nPragma: no-cache\r\nContent-Type: application/json\r\n\r\n");
}

void sendErrorJson(int sock, const std::string &error)
{
    rapidjson::Document json_doc;
    json_doc.SetObject();
    json_doc.AddMember("error", error, json_doc.GetAllocator());

    rapidjson::StringBuffer str_buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);
    json_doc.Accept(writer);

    sendJSONHeader(sock);
    writestring(sock, str_buf.GetString());
    close(sock);
}

int myDoPost(int sock, char *url, char *pData, char *rxBuffer)
{
	// Handle POST requests here

    return 0;
}

int myDoGet(int sock, PSTR url, PSTR rxBuffer)
{
    std::string str_url(url);
    std::string url_name = str_url.substr(0, str_url.find('?'));

    std::string query_string;
    std::size_t delim_pos = str_url.find('?');
    if (delim_pos != std::string::npos)
        query_string = str_url.substr(delim_pos + 1);

    rapidjson::Document json_doc;
    json_doc.SetObject();
    rapidjson::Document::AllocatorType &json_alloc = json_doc.GetAllocator();

    if (url_name == "index")
    {
		// TODO: unite this with the one from main.cpp
        auto printTemperature = [&] (const std::string &name) -> std::string {
            types::Temperature t = g_vars::temp_pool->temperature(name);
            if (t.is_valid)
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << t.value;
                return oss.str();
            }

            return std::string();
        };

        json_doc.AddMember("pu", printTemperature(constants::tsensornames::power_unit), json_alloc);
        json_doc.AddMember("m", printTemperature(constants::tsensornames::modem), json_alloc);
        json_doc.AddMember("cb", printTemperature(constants::tsensornames::cross_board), json_alloc);
        json_doc.AddMember("uc", printTemperature(constants::tsensornames::upconverter), json_alloc);

        rapidjson::StringBuffer str_buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);
        json_doc.Accept(writer);

        SendHTMLHeader(sock);
        writestring(sock, str_buf.GetString());
        return 0;
    }

    return (*oldGetHandler)(sock, url, rxBuffer);
}
