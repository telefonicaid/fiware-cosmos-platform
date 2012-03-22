#include <string>
#include <vector>
#include <Service.h>


class Service
{


    static char *strnstr_limitpattern(const char *text, const char *pattern, size_t max_length)
    {
        char cpat, ctxt;
        size_t len;

        if ((cpat = *pattern++) != '\0')
        {
            len = strlen(text);
            do
            {
                do
                {
                    if ((len-- < max_length) || (ctxt = *text++) == '\0')
                    {
                        return (NULL);
                    }
                } while (ctxt != cpat);
            } while (strncmp(text, pattern, max_length-1) != 0);
            text--;
        }
        return ((char *)text);
    }

    static bool matchPatterns (const char *inputString, const char *pattern, char wildcard)
    {
        const char *p_input;
        const char *p_pattern;
        char *p_wildcard;
        const char *p_match;


        // First, lets skip trivial situations
        if (inputString == NULL)
        {
            return false;
        }

        if (*inputString == '\0')
        {
            return false;
        }

        if (pattern == NULL)
        {
            return false;
        }

        if (*pattern == '\0')
        {
            return false;
        }

        p_input = inputString;
        p_pattern = pattern;
        while ((p_wildcard = strchr((char *)p_pattern, wildcard)) != NULL)
        {
            size_t len_pattern = p_wildcard - p_pattern;
            //LM_M(("wildcard at '%s', pattern:'%s' with len:%lu", p_wildcard, p_pattern, len_pattern));
            if ((p_match = strnstr_limitpattern(p_input, p_pattern, len_pattern)) == NULL)
            {
                //LM_M(("No match at '%s' with pattern:'%s' and len:%lu", p_input, p_pattern, len_pattern));
                return false;
            }
            p_input = p_match + len_pattern;
            p_pattern = p_wildcard + 1;
            if (*p_pattern == '\0')
            {
                return true;
            }
        }
        if (((p_match = strstr(p_input, p_pattern)) == NULL) || (*(p_match + strlen(p_pattern))) != '\0')
        {
            //LM_M(("No last match at '%s' with pattern:'%s' and len:%lu", p_input, p_pattern, strlen(p_pattern)));
            return false;
        }
        return true;
    }


    bool checkHTTP(std::string url)
    {
        char wildcard = '%';
        const char *p_url = url.c_str();
        for (unsigned int i = 0; i < httpPatterns.size(); i++)
        {
            if (matchPatterns(p_url, httpPatterns[i].c_str(), wildcard))
            {
                return true;

            }
        }
        return false;
    }

    bool checkDNS(std::string dns)
    {
        char wildcard = '%';
        const char *p_dns = dns.c_str();
        for (unsigned int i = 0; i < dnsPatterns.size(); i++)
        {
            if (matchPatterns(p_dns, dnsPatterns[i].c_str(), wildcard))
            {
                return true;

            }
        }
        return false;
    }



};

