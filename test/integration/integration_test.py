"""
    Integration tests for the Game Boy Emulator.

    @author: Viraj Mahesh
"""

import os
from subprocess import Popen, PIPE

PATH = os.path.dirname(os.path.realpath(__file__))
TEST_ROM_PATH = '{}/{}'.format(PATH, 'test_roms')

BOOT_TEST_ROM_PATH = '{}/{}'.format(TEST_ROM_PATH, 'boot')
CPU_TEST_ROM_PATH = '{}/{}'.format(TEST_ROM_PATH, 'cpu')
TIMING_TEST_ROM_PATH = '{}/{}'.format(TEST_ROM_PATH, 'timing')

INTEGRATION_BINARY_PATH = '../../build/bin/tests/IntegrationTest'


def run_test(rom_file, instructions):
    """
    Helper function for running integration tests.

    :param rom_file: The path to the ROM we want to run.
    :param instructions: The number of instructions we want to run the ROM for.
    :return: The hash of the screen buffer, as output by the integration test binary.
    """
    args = [INTEGRATION_BINARY_PATH, rom_file, str(instructions)]
    test = Popen(args, stdout=PIPE)

    test.wait()
    return test.stdout.read()


def test_blargg_timing_test():
    """
    Test that the emulator passes Blargg's timing test rom.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'timing_test.gb')
    screen_hash = run_test(rom_file, 300000)

    assert screen_hash == '806d2b02697'


def test_div_write():
    """
    Test that writes to DIV register correctly reset the internal clock and
    timing registers.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'div_write.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '7dce967813f'


def test_tima_increment_1():
    """
    Test that the TIMA register is correctly incremented in Timer Mode 1.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_increment_1.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '7ad768a3131'


def test_tima_div_trigger_1():
    """
    Test that writes to TMA are correctly handled.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_div_trigger_1.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '7ad768a3131'


def test_tima_increment_2():
    """
    Test that the TIMA register is correctly incremented in Timer Mode 2.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_increment_2.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '6af2d613bc4'


def test_tima_increment_3():
    """
    Test that the TIMA register is correctly incremented in Timer Mode 3.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_increment_3.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '7ad768a3131'


def test_tima_increment_4():
    """
    Test that the TIMA register is correctly incremented in Timer Mode 4.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_increment_4.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '7ad768a3131'


def test_tima_reload():
    """
    Test that the TIMA register is correctly reloaded after overflow.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_reload.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '8d02c9a80ac'


def test_tima_write_reload():
    """
    Test that writes to TIMA during reloading are correctly handled.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tima_write_reload.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '6f5f240884e'


def test_tma_write_reload():
    """
    Test that writes to TMA are correctly handled.
    """
    rom_file = '{}/{}'.format(TIMING_TEST_ROM_PATH, 'tma_write_reload.gb')
    screen_hash = run_test(rom_file, 500000)

    assert screen_hash == '6750a7f4a48'


def test_blargg_cpu_test():
    """
    Test that the emulator passes Blargg's cpu instruction test rom.
    """
    rom_file = '{}/{}'.format(CPU_TEST_ROM_PATH, 'instructions_test.gb')
    screen_hash = run_test(rom_file, 25000000)

    assert screen_hash == '717209ded53'


def test_boot_hwio():
    """
    Test that the Hardware Registers have correct boot values.
    """
    rom_file = '{}/{}'.format(BOOT_TEST_ROM_PATH, 'boot_hwio.gb')
    screen_hash = run_test(rom_file, 1000000)

    assert screen_hash == '7dce967813f'


def test_boot_regs():
    """
    Test that the CPU registers have correct boot values.
    """
    rom_file = '{}/{}'.format(BOOT_TEST_ROM_PATH, 'boot_regs.gb')
    screen_hash = run_test(rom_file, 1000000)

    assert screen_hash == '8b5d0c766cb'