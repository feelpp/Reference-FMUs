from itertools import product
from subprocess import check_call

import numpy as np
import pytest
from fmpy.util import read_csv


@pytest.mark.parametrize('fmi_version, interface_type', product([2, 3], ['cs', 'me']))
def test_start_time(executable, dist, work, fmi_version, interface_type):

    output_file = work / f'test_start_time_fmi{fmi_version}_{interface_type}.csv'

    fmu_filename = dist / f'{fmi_version}.0' / 'BouncingBall.fmu'

    check_call([
        executable,
        '--interface-type', interface_type,
        '--start-time', '0.5',
        r'--output-file', output_file,
        fmu_filename]
    )

    result = read_csv(output_file)

    assert result['time'][0] == 0.5


@pytest.mark.parametrize('fmi_version, interface_type', product([2, 3], ['cs', 'me']))
def test_stop_time(executable, work, dist, fmi_version, interface_type):

    output_file = work / f'test_stop_time_fmi{fmi_version}_{interface_type}.csv'

    fmu_filename = dist / f'{fmi_version}.0' / 'BouncingBall.fmu'

    check_call([
        executable,
        '--interface-type', interface_type,
        '--stop-time', '1.5',
        r'--output-file', output_file,
        fmu_filename]
    )

    result = read_csv(output_file)

    assert result['time'][-1] == 1.5


@pytest.mark.parametrize('fmi_version, interface_type', product([2, 3], ['cs', 'me']))
def test_input_file(executable, work, dist, resources, fmi_version, interface_type):

    input_file = resources / f'Feedthrough_in.csv'
    output_file = work / f'test_input_file_fmi{fmi_version}_{interface_type}.csv'

    fmu_filename = dist / f'{fmi_version}.0' / 'Feedthrough.fmu'

    check_call([
        executable,
        '--interface-type', interface_type,
        '--stop-time', '5',
        r'--input-file', input_file,
        r'--output-file', output_file,
        fmu_filename]
    )

    result = read_csv(output_file)

    t = result['time']

    # interpolated values
    x = result['Float64_continuous_output'][np.logical_and(t > 1.25, t < 1.75)]

    # start value
    assert result['Float64_continuous_output'][0] == 3

    # interpolation
    assert np.all(x > 2) and np.all(x < 3)

    # extrapolation
    assert result['Float64_continuous_output'][-1] == 3

    # start value
    assert result['Int32_output'][0] == 1

    # extrapolation
    assert result['Int32_output'][-1] == 2


@pytest.mark.parametrize('fmi_version, interface_type', product([2, 3], ['cs', 'me']))
def test_fmi_log_file(executable, work, dist, resources, fmi_version, interface_type):

    fmi_log_file = work / f'test_fmi_log_file_fmi{fmi_version}_{interface_type}.txt'

    fmu_filename = dist / f'{fmi_version}.0' / 'BouncingBall.fmu'

    check_call([
        executable,
        '--interface-type', interface_type,
        '--stop-time', '1.5',
        '--log-fmi-calls',
        '--fmi-log-file', fmi_log_file,
        fmu_filename]
    )

    assert fmi_log_file.is_file()


@pytest.mark.parametrize('fmi_version, interface_type', product([2, 3], ['cs', 'me']))
def test_output_interval(executable, work, dist, fmi_version, interface_type):

    output_file = work / f'test_output_interval_fmi{fmi_version}_{interface_type}.csv'

    fmu_filename = dist / f'{fmi_version}.0' / 'Dahlquist.fmu'

    check_call([
        executable,
        '--interface-type', interface_type,
        '--output-interval', '0.25',
        r'--output-file', output_file,
        fmu_filename]
    )

    result = read_csv(output_file)

    assert np.all(np.diff(result['time']) == 0.25)
