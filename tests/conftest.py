import pytest

def pytest_addoption(parser):
    parser.addoption("--reps", action="store", default=1, help="Number of times to run some of the tests.")
    parser.addoption("--fulltest", action="store_true", default=False, help="Run all tests (slow)")
    parser.addoption("--swo_trace", action="store_true", default=False, help="Enable trace tests.")
    parser.addoption("--xadc", action="store_true", default=False, help="Report XADC temp and status after each test")
    parser.addoption("--log", action="store_true", default=False, help="Log XADC stats to file")

@pytest.fixture
def fulltest(request):
    #return int(request.config.getoption("--fulltest"))
    return request.config.getoption("--fulltest")

@pytest.fixture
def swo_trace(request):
    return request.config.getoption("--swo_trace")

@pytest.fixture
def reps(request):
    return int(request.config.getoption("--reps"))

@pytest.fixture
def xadc(request):
    return int(request.config.getoption("--xadc"))

@pytest.fixture
def log(request):
    return int(request.config.getoption("--log"))

