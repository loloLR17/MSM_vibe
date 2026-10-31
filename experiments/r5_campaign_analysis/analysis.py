"""Exploratory R5 PC analysis. No device access and no normative weighting.
Acceptance of .TR2 belongs exclusively to the existing strict R3 reader.
"""
from pathlib import Path
import importlib.util
import numpy as np
from scipy import signal

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('strict_r3_reader', ROOT / 'experiments/r3_durable_fat32/tools/read_campaign.py')
reader = importlib.util.module_from_spec(spec)
spec.loader.exec_module(reader)
MG_PER_COUNT = .244
SETTLING_S = 5.0
DECIMATION = 16


def design(fs):
    return signal.butter(4, [2, 25], btype='bandpass', fs=fs, output='sos')


def verified_blocks(path, info, batch_records=4096):
    """Vectorized payload view AFTER strict acceptance; never an alternate verifier.
    Uses record endpoints exactly as the strict reader's integer interpolation.
    """
    with Path(path).open('rb') as f:
        f.seek(512)
        remaining = info['valid_records']
        index = 0
        while remaining:
            nrec = min(remaining, batch_records)
            buf = f.read(nrec * 512)
            if len(buf) != nrec * 512:
                raise ValueError('source changed after acceptance')
            ns = np.ndarray((nrec,), '<u2', buf, 36, (512,)).astype(np.int64)
            first = np.ndarray((nrec,), '<u8', buf, 16, (512,)).astype(np.int64)
            last = np.ndarray((nrec,), '<u8', buf, 24, (512,)).astype(np.int64)
            values = np.ndarray((nrec, 78, 3), '<i2', buf, 44, (512, 6, 2))
            j = np.arange(78)[None, :]
            keep = j < ns[:, None]
            times = first[:, None] + (last - first)[:, None] * j // np.maximum(ns[:, None] - 1, 1)
            x = values[keep].astype(np.float64) * MG_PER_COUNT
            t = (times[keep] - info['first_time_us']) * 1e-6
            yield index, t, x
            index += len(x)
            remaining -= nrec
        if index != info['valid_xyz']:
            raise ValueError('decoded count changed')


class BandStream:
    """Causal float64 SOS with explicit invalid samples and gap recovery.
    Constant-value steady-state initialization removes arbitrary DC startup step;
    first five seconds of each valid run remain invalid for band metrics.
    """
    def __init__(self, fs):
        self.fs, self.sos = fs, design(fs)
        self.zi = None
        self.age = 0
        self.last_time = None
        self.gaps = 0

    def apply(self, x, t, valid):
        out = np.full_like(x, np.nan, dtype=float)
        if not len(x):
            return out
        gaps = np.zeros(len(x), bool)
        if self.last_time is not None:
            gaps[0] = t[0] - self.last_time > 10 / self.fs
        gaps[1:] = np.diff(t) > 10 / self.fs
        self.gaps += int(gaps.sum())
        boundaries = np.unique(np.r_[0, len(x), np.flatnonzero(valid[1:] != valid[:-1]) + 1, np.flatnonzero(gaps)])
        for a, b in zip(boundaries[:-1], boundaries[1:]):
            if gaps[a]:
                self.zi, self.age = None, 0
            if not valid[a]:
                self.zi, self.age = None, 0
                continue
            if self.zi is None:
                self.zi = signal.sosfilt_zi(self.sos)[:, :, None] * x[a][None, None, :]
            y, self.zi = signal.sosfilt(self.sos, x[a:b], axis=0, zi=self.zi)
            settle = max(0, int(np.ceil(SETTLING_S * self.fs)) - self.age)
            out[a + min(settle, b-a):b] = y[min(settle, b-a):]
            self.age += b-a
        self.last_time = float(t[-1])
        return out


class Moments:
    """One-second sufficient statistics; coarser windows combine samples, not RMS averages."""
    def __init__(self, seconds):
        self.n = np.zeros(seconds, np.int64)
        self.ss = np.zeros((seconds, 3))
        self.peak = np.full((seconds, 3), -np.inf)
        self.vector = np.full(seconds, -np.inf)

    def add(self, t, x):
        good = np.isfinite(x).all(axis=1)
        ids = np.floor(t[good]).astype(np.int64)
        x = x[good]
        if not len(x):
            return
        starts = np.r_[0, np.flatnonzero(np.diff(ids)) + 1]
        bins = ids[starts]
        self.n[bins] += np.diff(np.r_[starts, len(x)])
        self.ss[bins] += np.add.reduceat(x*x, starts, axis=0)
        self.peak[bins] = np.maximum(self.peak[bins], np.maximum.reduceat(np.abs(x), starts, axis=0))
        self.vector[bins] = np.maximum(self.vector[bins], np.maximum.reduceat(np.sqrt(np.sum(x*x, axis=1)), starts))

    def groups(self, width):
        starts = np.arange(0, len(self.n), width)
        n = np.add.reduceat(self.n, starts)
        ss = np.add.reduceat(self.ss, starts, axis=0)
        rms = np.sqrt(np.divide(ss, n[:, None], out=np.full_like(ss, np.nan), where=n[:, None] > 0))
        peak = np.maximum.reduceat(self.peak, starts, axis=0)
        vector = np.maximum.reduceat(self.vector, starts)
        peak[~np.isfinite(peak)] = np.nan
        vector[~np.isfinite(vector)] = np.nan
        return dict(start_s=starts, n=n, rms=rms, global_rms=np.sqrt(np.sum(rms*rms, axis=1)), peak=peak, vector_peak=vector)


def velocity_spectral(x_mg, fs, low=1, high=80):
    """Unweighted periodic-signal illustration, mm/s; DC and out-of-band removed.
    Not a Wm implementation, instrument, or ISO/BV comfort indicator.
    """
    x = np.asarray(x_mg)
    freq = np.fft.rfftfreq(len(x), 1/fs)
    transfer = np.zeros(len(freq), complex)
    ok = (freq >= low) & (freq <= high)
    transfer[ok] = 9.80665 / (2j*np.pi*freq[ok])
    return np.fft.irfft(np.fft.rfft(x) * transfer, n=len(x))
