
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_OTTstream_parse_logs
#define _H_SAMSON_OTTstream_parse_logs


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/SimpleParser.h>

#include "Service.h"


namespace samson{
namespace OTTstream{


class parse_logs : public samson::system::SimpleParser
{

    bool (parse_logs ::*_parseFunction)( char*line, samson::system::UInt *userId, ServiceHit *hit);

    std::vector<char*> fields;
    char sep;

    std::vector<Service> services;

    samson::system::UInt userId;
    samson::system::UInt serviceId;
    samson::system::TimeUnix timestamp;
    samson::OTTstream::ServiceHit hit;

    std::string category;

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  output: system.UInt OTTstream.ServiceHit
    //
    // helpLine: parse input logs from http, and emit service use with userId as key
    //  END_INFO_MODULE


    uint64_t phone2number (char *field,  int max_client ){
        int _length_client = 0;
        int _pos = 0;
        char *_endptr;
        uint64_t phone = 0;

        _length_client = strlen(field);

        if(_length_client > max_client) {
            _pos = _length_client - max_client;
        } else {
            _pos = 0;
        }

        phone = strtoul( &(field[_pos]), &_endptr, 10 );

        if( strcmp( "", _endptr ) != 0 ){
            phone = 0L;
        }

        return phone;
    }


    uint64_t classify_http(char *url)
    {
        for (unsigned int i = 0; (i < services.size()); i++)
        {
            if (services[i].checkHTTP(url))
            {
                return services[i].serviceId;
            }
        }
        return 0;
    }

    uint64_t classify_dns(char *url)
    {
        for (unsigned int i = 0; (i < services.size()); i++)
        {
            if (services[i].checkDNS(url))
            {
                return services[i].serviceId;
            }
        }
        return 0;
    }

    bool parseLine_HTTP( char * line, samson::system::UInt *userId, ServiceHit *hit )
    {
        int maxDigits = 9;

        userId->value = 0;
        hit->serviceId.value = 0;

        char *p_idString;

        split_in_words( line, fields, sep);

        if (fields.size() < 16)
        {
            return false;
        }
        userId->value = phone2number(fields[6], maxDigits);

        if (userId->value == 0)
        {
            return false;
        }

        if (strcmp(category.c_str(), "category") == 0)
        {
            p_idString = fields[2];
        }
        else
        {
            p_idString = fields[1];
        }
        hit->serviceId.value = classify_http(p_idString);

        if (hit->serviceId.value == 0)
        {
            return false;
        }


        hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[9]);

        return true;

    }

    bool parseLine_DNS( char * line, samson::system::UInt *userId, ServiceHit *hit )
    {
        int maxDigits = 9;
        userId->value = 0;
        hit->serviceId.value = 0;

        char *p_idString;

        split_in_words( line, fields, sep);

        if (fields.size() < 16)
        {
            return false;
        }
        userId->value = phone2number(fields[6], maxDigits);

        if (userId->value == 0)
        {
            return false;
        }


        if (strcmp(category.c_str(), "category") == 0)
        {
            p_idString = fields[2];
        }
        else
        {
            p_idString = fields[1];
        }
        hit->serviceId.value = classify_dns(p_idString);

        if (hit->serviceId.value == 0)
        {
            return false;
        }


        hit->timestamp.setFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[9]);

        return true;


    }

    void init( samson::KVWriter *writer )
    {
        std::string source = environment->get("OTTstream.source", "HTTP");                // values: HTTP or DNS

        if (source == "HTTP")
        {
            _parseFunction = &parse_logs::parseLine_HTTP;
        }
        else
        {
            _parseFunction = &parse_logs::parseLine_DNS;
        }

        category   = environment->get("OTTstream.http_category", "category"); //values: category or subcategory
        sep = '\t';

        {
            Service newService("Skype", 10);
            newService.addHTTPPattern("http://ui.skype.com/%/getlatestversion?%");
            newService.addHTTPPattern("http://apps.skype.com/countrycode");
            newService.addHTTPPattern("http://conn.skype.com");
            services.push_back(newService);
        }
        {
            Service newService("Viber", 20);
            newService.addHTTPPattern("http://www.cdn.viber.com/ok.txt");
            newService.addHTTPPattern("http://www.cdn.viber.com/android_version.txt");
            services.push_back(newService);
        }
        {
            Service newService("Tango", 80);
            newService.addDNSPattern("%.cm.tango.me");
            services.push_back(newService);
        }
        {
            Service newService("GTalk", 40);
            newService.addHTTPPattern("talkgadget.google.com");
            newService.addHTTPPattern("talkgadget.google");
            newService.addHTTPPattern("hostedtalkgadget.google.com");
            newService.addHTTPPattern("dl.google.com");
            newService.addHTTPPattern("tc.v%.cache%.c.pack.google.com");
            newService.addDNSPattern("talkgadget.google.com");
            newService.addDNSPattern("talkgadget.l.google.com");
            newService.addDNSPattern("hostedtalkgadget.google.com");
            newService.addDNSPattern("%.talkgadget.google.com");
            newService.addDNSPattern("mtalk.google.com");
            newService.addDNSPattern("talk.google.com");
            newService.addDNSPattern("mtalk.google.com.%");
            newService.addDNSPattern("talk.google.com.%");
            newService.addDNSPattern("talk.%.google.com");
            services.push_back(newService);
        }
        {
            Service newService("WhattsApp", 50);
            newService.addDNSPattern("sro.whatsapp.net");
            newService.addDNSPattern("bin-short.whatsapp.net");
            newService.addDNSPattern("bin-nokia.whatsapp.net");
            newService.addDNSPattern("mms.whatsapp.net");
            newService.addDNSPattern("mms40%.whatsapp.net");
            newService.addDNSPattern("mms30%.whatsapp.net");
            newService.addDNSPattern("mms20%.whatsapp.net");
            services.push_back(newService);
        }
        {
            Service newService("faceTime", 60);
            services.push_back(newService);
        }
        {
            Service newService("iphoneMessage", 70);
            services.push_back(newService);
        }
        {
            Service newService("Facebook", 30);
            newService.addHTTPPattern("http://www.facebook.com/ajax/messaging/typ.php?__a=1");
            newService.addHTTPPattern("http://apps.facebook.com/ajax/messaging/typ.php?__a=1");
            newService.addHTTPPattern("http://es-es.facebook.com/ajax/messaging/typ.php?__a=1");
            newService.addHTTPPattern("http://www.facebook.com/ajax/messaging/async.php?__a=1");
            newService.addHTTPPattern("http://apps.facebook.com/ajax/messaging/async.php?__a=1");
            newService.addHTTPPattern("http://es-es.facebook.com/ajax/messaging/async.php?__a=1");
            newService.addHTTPPattern("http://www.facebook.com/ajax/messaging/send.php?__a=1");
            newService.addHTTPPattern("http://apps.facebook.com/ajax/chat/send.php?__a=1");
            newService.addHTTPPattern("http://es-es.facebook.com/ajax/chat/send.php?__a=1");
            newService.addHTTPPattern("http://www.facebook.com/ajax/chat/buddy_list.php?__a=1");
            newService.addHTTPPattern("http://apps.facebook.com/ajax/chat/buddy_list.php?__a=1");
            newService.addHTTPPattern("http://es-es.facebook.com/ajax/chat/buddy_list.php?__a=1");
            newService.addHTTPPattern("http://www.facebook.com/ajax/chat/tabs.php?__a=1");
            newService.addHTTPPattern("http://apps.facebook.com/ajax/chat/tabs.php?__a=1");
            newService.addHTTPPattern("http://touch.facebook.com/touch/chathistory.php");
            newService.addHTTPPattern("http://iphone.facebook.com/touch/chathistory.php");
            newService.addHTTPPattern("http://www.facebook.com/ajax/presence/update.php");
            newService.addHTTPPattern("http://%.channel.facebook.com");
            services.push_back(newService);
        }
        {
            Service newService("Tuenti", 90);
            newService.addHTTPPattern("m.tuenti.com");
            newService.addHTTPPattern("xmpp%.tuenti.com");
            newService.addHTTPPattern("api.tuenti.com");
            newService.addHTTPPattern("fotos.api.tuenti.com");
            newService.addHTTPPattern("api.pl.tuenti.com");
            services.push_back(newService);
        }
        {
            Service newService("Google+", 100);
            services.push_back(newService);
        }
        {
            Service newService("TEST", 200);
            newService.addHTTPPattern("www.c%n.v%ber.com");
            newService.addDNSPattern("www.applesfera.com");
            newService.addDNSPattern("www.applesfera.com");
            services.push_back(newService);
        }




    }


    void parseLine( char * line, samson::KVWriter *writer )
    {

        if ((this->*_parseFunction)(line, &userId, &hit))
        {
            writer->emit(0, &userId, &hit);
        }


    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace OTTstream
} // end of namespace samson

#endif
