import time

### Handles string management coming in from the serial
###   performs string operations and manages the serial
###   timer stuff
### 
### This code will be part of where everything will go wrong
###   when the MESC terminal changes the format of its data

class PayloadError(Exception):
    """A custom exception used to report errors in use of Timer class"""

class Payload:
    def __init__(self):
        self._start_time = None
        self._string = ''
        self._struct = None

    def concatString(self, s):
        """add to string"""
        if self._string is None:
            raise PayloadError(f"init string to add to it")

        self._string += s

    def setString(self, s):
        """add to string"""
        if self._string is None:
            raise PayloadError(f"init string to set it")

        self._string = s

    def resetString(self):
        """set string to empty"""
        print("PAYLOAD reset")
        if self._string is None:
            raise PayloadError(f"init string to reset it")

        self._string = ''
        self._struct = None

    def reportString(self):
        """send string value"""
        if self._string is None:
            raise PayloadError(f"init string to report it")

        return(self._string)

    def startTimer(self):
        """Start a new timer"""
        if self._start_time is not None:
            raise PayloadError(f"timer is running. Use .stop() to stop it")

        self._start_time = time.perf_counter()

    def stopTimer(self):
        """Stop the timer, and report the elapsed time"""
        if self._start_time is None:
            raise PayloadError(f"timer is not running. Use .start() to start it")

        elapsed_time = time.perf_counter() - self._start_time
        self._start_time = None

    def resetTimer(self):
        """Reset a new timer"""
        if self._start_time is None:
            raise PayloadError(f"timer is not running. Use .start() to start it")

        self._start_time = time.perf_counter()


    def reportTimer(self):
        """return timer value"""
        if self._start_time is None:
            raise PayloadError(f"timer is not running. Use .start() to start it")

        return(time.perf_counter() - self._start_time)

    def parsePayload(self):
        count = 0
        start = False

        self._goodPayload = False

        self._struct = {}
        self._struct['names'] = []

        lines = self._string

        for line in lines.split('\n'):
            count += 1
            if start: 
                l = line.split('\t')
                if len(l) == 5: # the only way we know we're getting variables
                    # print("Line {}: {}".format(line.count('\t'), line.strip()))
                    self._struct[l[0]] = {}
                    self._struct[l[0]]['value'] = l[1]
                    self._struct[l[0]]['min'] = l[2]
                    self._struct[l[0]]['max'] = l[3]
                    self._struct[l[0]]['desc'] = l[4]
                    self._struct['names'].append(l[0])
            if "Parameter\tValue" in line:
                start = True

    def reportPayload(self):
        """return payload structure"""
        # notice this clears the struct
        tmp = self._struct
        self._struct = {}
        return(tmp)
