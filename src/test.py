import os
import argparse
import multiprocessing
from shutil import which
import itertools
import datetime
import time
import glob
import subprocess
# get a recursive list of file paths that matches pattern including sub directories

 
# Iterate over the list of filepaths & remove each file.

def find_score(filename, instance):
    with open(f"logs/{instance}/{filename}", "r") as f:
        for line in f.readlines():
            if "Best solution:" in line:
                return line.split()[2]
        return 0

def is_running():
    try:
        subprocess.check_output(["pgrep", "main"])
    except:
        return 0
    else:
        return 1

total_instances=["instance01", "instance02", "instance03", "instance04", "instance05", "instance06", "instance07", "instance08" ]
def_alpha = [0.5, 10, 50, 100]
def_mutations = [2, 7, 12]
def_cooling = [0.75, 0.85, 0.9, 0.95]

parser = argparse.ArgumentParser(description='Grid search for hyperparameters.')

parser.add_argument('--program', metavar='program_name', default="main", type=str, nargs=1, dest='program_name',
                    help='a string indicating the program to be run. If missing, main will be used.')
parser.add_argument('-i', metavar='instance', type=str, default=total_instances, nargs='+', dest='instance',
                    help='a string indicating the instance. If missing, all instances will be used.')
parser.add_argument('-t', metavar='runtime', type=int, default=180, nargs=1, dest='runtime',
                    help='an integer for the program run time in s. If missing, 180s (3m) will be used.')
parser.add_argument('--clean-logs', action="store_true", default=False, dest='clean',
                    help='a flag to clean previous log files')
parser.add_argument('--alpha', metavar='alpha', type=float, default=def_alpha, nargs='+', dest='alpha',
                    help='a list of floats to be tried as alpha values.')
parser.add_argument('--mutations', metavar='mutations', type=int, default=def_mutations, nargs='+', dest='mutations',
                    help='a list of ints to be tried as number of mutations.')
parser.add_argument('--cooling', metavar='cooling', type=float, default=def_cooling, nargs='+', dest='cooling',
                    help='a list of floats to be tried as cooling coefficients.')
parser.add_argument('--shutdown', action="store_true", default=False, dest='shutdown',
                    help='a flag shutdown the machine at the end of the task (SUGGESTED FOR GOOGLE CLOUD VM)')

parallel = multiprocessing.cpu_count()/4

args = parser.parse_args()
with open("./logs/test.log", 'w') as log:
    if isinstance(args.program_name, list):
        program = args.program_name[0]
    elif isinstance(args.program_name, str):
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Program not specified. Using main as default\n")
        program = args.program_name


    if isinstance(args.runtime, list):
        runtime = args.runtime[0]
    elif isinstance(args.runtime, int):
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Runtime not specified. Using 180 as default\n")
        runtime = args.runtime

    instances = args.instance
    clean = args.clean
    shutdown = args.shutdown

    alpha = args.alpha
    mutations = args.mutations
    cooling = args.cooling



    if os.path.isfile(f"./{program}"): 
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Program {program} found\n")
    else:
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Program {program} not found. Trying to compile {program}\n")
        if which('g++-9') is not None:
            log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Compiling with g++-9\n")
            try:
                if os.system(f"g++-9 -g -ggdb -o {program} {program}.cpp -fopenmp") != 0:
                    raise Exception(f"An error occured. Program {program} not compiled")
            except: 
                log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- An error occured. Program {program} not compiled\n")
            else:
                log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Compiled successfully\n")
        elif which('g++-7') is not None:
            log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Compiling with g++-7\n")
            try:
                if os.system(f"g++-7 -g -ggdb -o {program} {program}.cpp -fopenmp") != 0:
                    raise Exception(f"An error occured. Program {program} not compiled")
            except: 
                log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- An error occured. Program {program} not compiled\n")
            else:
                log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Compiled successfully\n")



    # TEST VALUES
    # alpha = [0.5, 10]
    # mutations = [2, 7]
    # cooling = [0.75]

    combinations = list(itertools.product(alpha, mutations, cooling))
    i=0

    if not os.path.exists('logs'):
        os.makedirs('logs')


    log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Running {parallel} parallel instances with arguments: {program} {instances} {runtime}\nalpha: {alpha}\nmutations: {mutations}\ncooling: {cooling}\n")
    runs = {}
    for instance in instances:
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] ------ INSTANCE {instance} BEGIN ------\n")
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Running grid search on instance {instance}\n")
        # CREATE FOLDERS
        if not os.path.exists(f'logs/{instance}'):
            os.makedirs(f'logs/{instance}')
        if not os.path.exists(f'logs/{instance}/best'):
            os.makedirs(f'logs/{instance}/best')
        if not os.path.exists(f'logs/{instance}/errors'):
                    os.makedirs(f'logs/{instance}/errors')
        
        # CLEAR PREVIOUS LOGS IF SELECTED WITH --clean-logs
        if clean:
            fileList = glob.glob(f'./logs/{instance}/log_*', recursive=True)
            for filePath in fileList:
                os.remove(filePath)
            fileList = glob.glob(f'./logs/{instance}/best/log_*', recursive=True)
            for filePath in fileList:
                os.remove(filePath)
            fileList = glob.glob(f'./logs/{instance}/errors/log_*', recursive=True)
            for filePath in fileList:
                os.remove(filePath)
        # RUN ALL COMBINATIONS
        for comb in combinations:
            a=comb[0]
            mut=comb[1]
            cool=comb[2]
            
            if i == parallel:
                time.sleep(runtime + 1)
                i=0
            filename=f"log_{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}_alpha{a}_mut{mut}_cool{cool}.txt"
            log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Running with alpha: {a}, cool: {cool}, mut: {mut}\n")
            os.system(f"./{program} {instance} {runtime} 4 {a} {mut} {cool} > logs/{instance}/{filename} &")
            if instance not in runs.keys():
                runs[instance] = [filename]
            else: 
                runs[instance].append(filename)
            i+=1

        # WAIT FOR LAST INSTANCES TO FINISH
        time.sleep(runtime+1)

    # WAIT FOR ALL INSTANCES TO FINISH SINCE TIMER DEPENDS ON INITIAL SOLUTION
    res = is_running()
    while res:
        res = is_running()
        time.sleep(1)
    log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] --------- All instances are done computing\n")
    for instance in instances:
        best = False
        best_score = 9999
        log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] --------- Parsing solutions for instance {instance}\n")
        for filename in runs[instance]:
        # LOOK FOR BEST SOLUTION
            score = float(find_score(filename, instance))
            if score != 0:
                if score < best_score:
                    best = True
                    best_score = score
                    best_file = filename
            else: #IF AN INSTANCE DID NOT FINISH, PLACE ITS LOG IN logs/instance0#/errors
                os.rename(f"logs/{instance}/{filename}", f"logs/{instance}/errors/{filename}")
                log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] --ERROR-- Instance {instance} interrupted, no score provided by {filename}\n")
        if best:
            log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] -- Best solution for instance {instance}: {best_score} | {best_file}\n")
            # MOVE THE BEST TO logs/instance0#/best
            os.rename(f"logs/{instance}/{best_file}", f"logs/{instance}/best/{best_file}")
        # log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] ------ INSTANCE {instance} END ------\n")

    log.write(f"[{datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}] --------- END ---------\n")

# SHUTDOWN (FOR GCLOUD VM)
if os.name == 'posix' and shutdown:
    os.system("shutdown -h now")