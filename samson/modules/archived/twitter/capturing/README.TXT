To get tweets in stream, the gettweets.py python program may be used.
You'll need python, the curl library, and pycurl and json modules for python.

python gettweets.py  | samsonPush -v -buffer_size 10000 -timeout 1 -mr 1000000 hit.01.in_txt
