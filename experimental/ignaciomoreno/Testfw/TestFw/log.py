"""
"""

import logging

class Log(object):
    """
    """

    def __init__(self, filePath = './moduleTestData/Log.log', level = 'DEBUG', format = '%(asctime)-15s [%(module)s] %(levelname)s - %(message)s'):
        """
        """
#format = '%(asctime)-15s [%(module)s] %(funcName)s %(levelname)s - %(message)s'
        self.filePath = filePath
        self.format = format
        self.logLevel = level
        logging.basicConfig(format = self.format, filename=self.filePath, \
# filemode='w', \
 level = logging.DEBUG)
#        """ #For both logging in console and file:
        console = logging.StreamHandler()
        console.setLevel(logging.INFO)
        # set a format which is simpler for console use
        formatter = logging.Formatter(self.format)
        # tell the handler to use this format
        console.setFormatter(formatter)
        # add the handler to the root logger
        logging.getLogger('').addHandler(console)
 #       """
        self.extraAtt = {'funName': str(__name__)}
        logging.info('LOG BEGINNING')

        """
    def log(self, text):
        """
        """
        logging.log(str(text))
        """

    def debug(self, text):
        """
        """
        logging.debug(str(text))

    def info(self, text):
        """
        """
        logging.info(str(text))

    def warning(self, text):
        """
        """
        logging.warning(str(text))

    def error(self, text):
        """
        """
        logging.error(str(text))

    def critical(self, text):
        """
        """
        logging.critical(str(text))

    def __del__(self):
        """
        """
        logging.info("LOG END.")

def testMethod():
    log = Log(str('./moduleTestData/logTestOut.log'))
#    log = Log() # What if a second log is instantiated???#
    log.info("This is just mere information.")
    log.debug("This is debug information.")
    log.warning("This is a warn, next time it will be a threat.")
    log.error("This is an error, your job is really in peril.")
    log.critical("This is a critical error, consider yourself laid out.")


if __name__ == '__main__':
    testMethod()


"""
http://docs.python.org/howto/logging.html#logging-basic-tutorial
http://docs.python.org/library/logging.html#logging.Formatter
http://docs.python.org/release/2.5.2/lib/multiple-destinations.html
http://stackoverflow.com/questions/251464/how-to-get-the-function-name-as-string-in-python
http://stackoverflow.com/questions/510972/getting-the-class-name-of-an-instance-in-python
http://stackoverflow.com/questions/251464/how-to-get-the-function-name-as-string-in-python
http://docs.python.org/library/logging.html#logging.Formatter
http://docs.python.org/howto/logging.html#logging-basic-tutorial
"""
