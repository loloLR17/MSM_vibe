"""No device access: compatibility success/fail are exercised with injected environments."""
import unittest
from types import SimpleNamespace
from unittest.mock import patch
import preflight_hp


class PreflightTests(unittest.TestCase):
    def test_windows_dependency_and_offbus_modes(self):
        def forbidden(*args, **kwargs):
            raise AssertionError("Serial must not be instantiated")
        serial = SimpleNamespace(__version__="3.5", __file__="fake", Serial=forbidden)
        with patch.object(preflight_hp.sys, "platform", "win32"), patch.object(preflight_hp.importlib, "import_module", return_value=serial):
            result = preflight_hp.inspect_environment()
        self.assertTrue(result["environment_ready"])
        self.assertFalse(result["campaign_go"])
        self.assertFalse(result["port_opened"])
        self.assertEqual((result["mock_nominal"], result["mock_silences"]), (10, 2))

    def test_missing_dependency_and_wrong_host_are_no_go(self):
        with patch.object(preflight_hp.sys, "platform", "linux"), patch.object(preflight_hp.importlib, "import_module", side_effect=ImportError("missing serial")):
            result = preflight_hp.inspect_environment()
        self.assertFalse(result["environment_ready"])
        self.assertEqual(len(result["errors"]), 2)
        self.assertTrue(result["offline_ok"] and result["mock_ok"])

    def test_wrong_package_version_is_rejected(self):
        serial = SimpleNamespace(__version__="wrong", Serial=lambda: None)
        with patch.object(preflight_hp.sys, "platform", "win32"), patch.object(preflight_hp.importlib, "import_module", return_value=serial):
            self.assertFalse(preflight_hp.inspect_environment()["environment_ready"])


if __name__ == "__main__":
    unittest.main()
