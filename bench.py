#!/usr/bin/env python2
import logging
import subprocess
import time
import sys
from logging.handlers import RotatingFileHandler

benchtype = sys.argv[1:]

args = ["-c", "ffffc0000",
        "-n", "4",
        "--socket-mem", "0,1024",
        "--"] + benchtype

#----------------------------------------------------------------------
def create_rotating_log(path):
    """
    Creates a rotating log with numbered suffix
    """
    logger = logging.getLogger("Rotating Log")
    logger.setLevel(logging.INFO)
 
    handler = RotatingFileHandler(path, maxBytes=20, backupCount=50)
    logger.addHandler(handler)
 
    return logger
 
#----------------------------------------------------------------------
def bench(logger):
    proc = subprocess.Popen(['./build/app/test-rte_ring'] + args,
            stdout=subprocess.PIPE)

    proc.wait()
    output = proc.stdout.readlines()

    logger.info("".join(output))
    print("\n".join(filter(lambda s:s.startswith("Throughput"), output)))

#----------------------------------------------------------------------
if __name__ == "__main__":
    log_file = "bench.log"
    logger = create_rotating_log(log_file)
    
    bench(logger)
