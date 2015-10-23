#!/usr/bin/env python2
import logging
import os
import subprocess
import sys
import time
from logging.handlers import RotatingFileHandler

SUDO = "sudo"
BIN_NAME = "./build/app/test-rte_ring"

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
def bench(basic_args, bench_type, logger):
    bench_result = {}
    proc = subprocess.Popen([SUDO, BIN_NAME] + basic_args
            + bench_type, stdout=subprocess.PIPE)

    proc.wait()
    output = proc.stdout.readlines()

    # parse and collect benchmark results
    logger.info("".join([" ".join(bench_type), '\n'] + output))

    bench_result["type"] = " ".join(bench_type)
    bench_result["throughput"] =  \
        map(lambda line: int(line.split()[-2].replace(",", "")),
            filter(lambda s:"Throughput" in s, output))

    bench_result["enq_cas_tries"] =  \
        map(lambda line: int(line.split()[-1].replace(",", "")),
                filter(lambda s:"EnQ CAS Tries" in s, output))

    bench_result["deq_cas_tries"] =  \
        map(lambda line: int(line.split()[-1].replace(",", "")),
                filter(lambda s:"DeQ CAS Tries" in s, output))

    return bench_result

#----------------------------------------------------------------------
if __name__ == "__main__":
    basic_args = ["-c", "ffffc0000",
                  "-n", "4",
                  "--socket-mem", "0,1024",
                  "--"]

    log_dir = "bench-output"
    log_file = "bench.output"
    log_path = os.path.join(log_dir, log_file)

    if not os.path.exists(log_dir):
            os.makedirs(log_dir)

    logger = create_rotating_log(log_path)


    bench_types = []
    for multi_threadness in ["-s"]:
        for bulk_size in [2**i for i in range(0, 1)]:
            if multi_threadness == "-m":
                for num_threads in range(1,9):
                    bench_types.append([multi_threadness, str(num_threads),
                                        "-b", str(bulk_size)])
            else:
                bench_types.append([multi_threadness, "-b", str(bulk_size)])


    results = {}
    for bench_type in bench_types:
        result = bench(basic_args, bench_type, logger)
        print(result)
        bench_type_str = " ".join(bench_type)
        results[bench_type_str] = result

    print(results)
