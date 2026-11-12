#!/usr/bin/env python3
"""Tests for tools/update_version.py (run: python3 -B tests/test_update_version.py)."""

import pathlib
import shutil
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import update_version  # noqa: E402

HEADER = ROOT / "common" / "vscp-firmware-version.h"


class UpdateVersionTest(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.mkdtemp()
        self.header = pathlib.Path(self.dir) / "vscp-firmware-version.h"
        shutil.copy(HEADER, self.header)

    def tearDown(self):
        shutil.rmtree(self.dir)

    def set_version(self, version):
        self.header.write_text(update_version.write_version(self.header.read_text(), version))

    def run_update(self, date):
        update_version.main(["--header", str(self.header), "--date", date])
        return update_version.read_version(self.header.read_text())

    def test_repository_header_is_valid(self):
        update_version.read_version(HEADER.read_text())

    def test_same_month_increments_patch(self):
        self.set_version((26, 11, 4))
        self.assertEqual((26, 11, 5), self.run_update("2026-11"))
        self.assertEqual((26, 11, 6), self.run_update("2026-11"))

    def test_new_month_resets_patch(self):
        self.set_version((26, 11, 17))
        self.assertEqual((26, 12, 0), self.run_update("2026-12"))

    def test_new_year_resets_patch(self):
        self.set_version((26, 12, 9))
        self.assertEqual((27, 1, 0), self.run_update("2027-01"))

    def test_string_and_number_follow(self):
        self.set_version((26, 11, 2))
        self.run_update("2026-11")
        text = self.header.read_text()
        self.assertIn('VSCP_FIRMWARE_LIB_VERSION_STR   "26.11.3"', text)
        self.assertIn("VSCP_FIRMWARE_LIB_VERSION       0x1A0B0003UL", text)

    def test_date_before_version_is_refused(self):
        self.set_version((26, 11, 0))
        with self.assertRaises(ValueError):
            self.run_update("2026-10")

    def test_check_does_not_change(self):
        before = self.header.read_text()
        update_version.main(["--header", str(self.header), "--check"])
        self.assertEqual(before, self.header.read_text())


if __name__ == "__main__":
    unittest.main()
