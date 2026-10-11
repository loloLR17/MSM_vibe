import base64
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import run_hp


class OrchestrationTests(unittest.TestCase):
    def test_fixed_client_and_noninteractive_host_validation(self):
        cmd, payload = run_hp.command('Inventory')
        self.assertEqual(cmd[0], run_hp.SSH)
        self.assertIn('BatchMode=yes', cmd)
        self.assertIn('StrictHostKeyChecking=yes', cmd)
        self.assertIn('tr2-hp', cmd)
        self.assertLess(len(cmd[-1]), 1000)
        self.assertIn(b"-Task 'Inventory'", payload)
        decoded = base64.b64decode(cmd[-1].split()[-1]).decode('utf-16le')
        self.assertIn('ReadToEnd', decoded)

    def test_quotes_and_no_arbitrary_task(self):
        self.assertEqual(run_hp.ps_quote("a'b"), "'a''b'")
        with self.assertRaises(ValueError):
            run_hp.command('Transmit')
        with self.assertRaises(ValueError):
            run_hp.command('Offline')

    def test_propagate_failure_and_preserve_binary_logs(self):
        with tempfile.TemporaryDirectory() as root:
            out = Path(root)/'proof'
            with patch('run_hp.subprocess.run', return_value=subprocess.CompletedProcess([],17,b'out',b'err')) as call:
                self.assertEqual(run_hp.run('FailureProbe',out),17)
                self.assertIn('input',call.call_args.kwargs)
            self.assertEqual((out/'stdout.log').read_bytes(),b'out')
            with self.assertRaises(FileExistsError):
                run_hp.run('Probe',out)

    def test_timeout_no_retry_and_unknown_remote_state(self):
        with tempfile.TemporaryDirectory() as root:
            out=Path(root)/'proof'
            with patch('run_hp.subprocess.run',side_effect=subprocess.TimeoutExpired('ssh',1,output=b'partial')) as call:
                self.assertEqual(run_hp.run('Probe',out,timeout=1),124)
                self.assertEqual(call.call_count,1)
            self.assertFalse(json.loads((out/'result.json').read_text())['remote_completion_known'])
            self.assertEqual((out/'stdout.log').read_bytes(),b'partial')

    def test_missing_client_and_ssh_failure(self):
        with tempfile.TemporaryDirectory() as root:
            with patch('run_hp.subprocess.run',side_effect=FileNotFoundError('missing ssh')):
                self.assertEqual(run_hp.run('Probe',Path(root)/'missing'),127)
            out=Path(root)/'network'
            with patch('run_hp.subprocess.run',return_value=subprocess.CompletedProcess([],255,b'',b'network')):
                self.assertEqual(run_hp.run('Probe',out),255)
            self.assertFalse(json.loads((out/'result.json').read_text())['remote_completion_known'])

    def test_offline_command_never_activates_serial(self):
        text=run_hp.SCRIPT.read_text()
        self.assertIn("--diagnostic --port COM7 --baud 115200 --parity Even --timeout-ms 1000",text)
        self.assertNotIn('--read-once',text)
        self.assertNotIn('SerialPort]::new',text)
        self.assertIn('Global\\TR2-COM7-Orchestration',text)
        self.assertIn('WaitForExit(15000)',text)


if __name__=='__main__':
    unittest.main()
