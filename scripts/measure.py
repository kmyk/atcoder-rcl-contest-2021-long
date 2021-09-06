"""
scripts/measure.py

A script to measure the efficacy of solutions rapidly.
"""

__author__ = "Kimiyuki Onaka"
__copyright__ = "Copyright (c) 2021 Kimiyuki Onaka"
__credits__ = []  # type: List[str]
__license__ = "MIT"
__version__ = "1.2.0"
__maintainer__ = "Kimiyuki Onaka"
__email__ = "kimiyuki95@gmail.com"
__status__ = "Production"

import argparse
import concurrent.futures
import math
import pathlib
import subprocess
import sys
from logging import DEBUG, basicConfig, getLogger
from typing import *

logger = getLogger(__name__)


def gen(*, seed: int) -> bytes:
    logger.info('running the generator...')
    command = [sys.executable, str(pathlib.Path('tester', 'generator.py')), str(seed)]
    return subprocess.check_output(command)


def run(*, command: str, input_path: pathlib.Path, output_path: pathlib.Path, seed: int) -> None:
    logger.info('running the command for seed %d...', seed)
    try:
        with open(input_path) as fh1:
            with open(output_path, 'w') as fh2:
                subprocess.check_call(command, stdin=fh1, stdout=fh2)
    except subprocess.SubprocessError:
        logger.exception('failed for seed = %d', seed)


def judge(*, input_path: pathlib.Path, output_path: pathlib.Path, seed: int) -> int:
    logger.info('running the visualizer for seed %d...', seed)
    try:
        command = [sys.executable, str(pathlib.Path('tester', 'judge.py')), str(input_path), str(output_path)]
        score_bytes = subprocess.check_output(command)
    except subprocess.SubprocessError:
        logger.exception('failed for seed = %d', seed)
        return 0
    if not score_bytes.startswith(b'score:'):
        raise RuntimeError(score_bytes.decode())
    return int(score_bytes.split(b':')[1])


def main() -> 'NoReturn':
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--command', default='./a.out')
    parser.add_argument('-n', '--count', type=int, default=50)
    parser.add_argument('-j', '--jobs', type=int, default=2)
    parser.add_argument('--same', action='store_true')
    parser.add_argument('--seed', type=int, default=0)
    args = parser.parse_args()

    basicConfig(level=DEBUG)

    if not pathlib.Path('tester').exists():
        logger.error('tester/ directory is not found')
        sys.exit(1)

    pathlib.Path('in').mkdir(exist_ok=True)
    pathlib.Path('out').mkdir(exist_ok=True)

    # gen
    if args.same:
        seeds = [args.seed for i in range(args.count)]
    else:
        seeds = [args.seed + i for i in range(args.count)]
    for seed in seeds:
        input_path = pathlib.Path('in', 'input_%d.txt' % seed)
        if not input_path.exists():
            input_data = gen(seed=seed)
            with open(input_path, 'wb') as fh:
                fh.write(input_data)

    # run
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        for i, seed in enumerate(seeds):
            input_path = pathlib.Path('in', 'input_%d.txt' % i)
            output_path = pathlib.Path('out', 'output_%d.txt' % i)
            executor.submit(run, command=args.command, input_path=input_path, output_path=output_path, seed=seed)

    # vis
    scores: List[int] = []
    for i, seed in enumerate(seeds):
        input_path = pathlib.Path('in', 'input_%d.txt' % i)
        output_path = pathlib.Path('out', 'output_%d.txt' % i)
        score = judge(input_path=input_path, output_path=output_path, seed=seed)
        scores.append(score)
        logger.info('seed = {}: score = {}'.format(seed, score))
    average = sum(scores) / len(scores)
    if args.same:
        logger.info('average = %s', average)
        logger.info('min = %s', min(scores))
        logger.info('max = %s', max(scores))
        logger.info('standard deviation = %s', math.sqrt(sum([(score - average)**2 for score in scores]) / len(scores)))
    else:
        logger.info('average = %s', average)

    if min(scores) <= 0:
        sys.exit(1)
    sys.exit(0)


if __name__ == '__main__':
    main()
