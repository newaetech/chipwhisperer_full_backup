import pytest

def pytest_addoption(parser):
    parser.addoption("--target", action="store", default="Lite", help="Pro or Lite")
    parser.addoption("--reps", action="store", default=1, help="Number of times to run each test function")
    parser.addoption("--loose", action="store_true", default=False, help="Loosen all the check margins")


