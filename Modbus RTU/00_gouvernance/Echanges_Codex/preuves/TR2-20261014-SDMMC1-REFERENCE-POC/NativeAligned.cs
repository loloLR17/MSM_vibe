using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
public sealed class NativeAlignedResult {
 public int WrittenBytes; public int ReadBytes; public bool FlushOK; public double WriteFlushMs; public double ReadMs; public int Mismatches; public int FirstMismatch=-1; public string ReadSHA256;
}
public static class NativeAligned {
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] static extern IntPtr CreateFileW(string path,uint access,uint share,IntPtr security,uint disposition,uint flags,IntPtr template);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool SetFilePointerEx(IntPtr file,long offset,out long result,uint origin);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool WriteFile(IntPtr file,IntPtr buffer,uint count,out uint done,IntPtr overlap);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool ReadFile(IntPtr file,IntPtr buffer,uint count,out uint done,IntPtr overlap);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool FlushFileBuffers(IntPtr file);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool CloseHandle(IntPtr file);
 [DllImport("kernel32.dll",SetLastError=true)] static extern IntPtr VirtualAlloc(IntPtr address,UIntPtr size,uint kind,uint protect);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool VirtualFree(IntPtr address,UIntPtr size,uint kind);
 static void Check(bool ok,string phase) { if(!ok) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error(),phase); }
 static IntPtr Open(string path,uint access,uint flags) {IntPtr h=CreateFileW(path,access,3,IntPtr.Zero,3,flags,IntPtr.Zero);Check(h!=new IntPtr(-1),"CreateFile");return h;}
 public static NativeAlignedResult Run(string path,long offset,byte[] payload,string dump) {
  if(payload.Length!=131072 || offset!=504256L*512 || offset%4096!=0) throw new ArgumentException("Fixed test bounds");
  NativeAlignedResult r=new NativeAlignedResult(); IntPtr buffer=VirtualAlloc(IntPtr.Zero,new UIntPtr(8192),0x3000,4); Check(buffer!=IntPtr.Zero,"VirtualAlloc");
  IntPtr h=new IntPtr(-1); byte[] read=new byte[payload.Length]; long at; uint done; Stopwatch timer=new Stopwatch();
  try {
   // NO_BUFFERING | WRITE_THROUGH, documented Windows flags. No disk policy or ACL change.
   h=Open(path,0xc0000000,0xa0000000);Check(SetFilePointerEx(h,offset,out at,0),"Seek write");timer.Start();
   for(int i=0;i<payload.Length;i+=8192) {Marshal.Copy(payload,i,buffer,8192);Check(WriteFile(h,buffer,8192,out done,IntPtr.Zero),"Write chunk "+(i/8192));Check(done==8192,"Short write");r.WrittenBytes+=(int)done;}
   r.FlushOK=FlushFileBuffers(h);Check(r.FlushOK,"FlushFileBuffers");timer.Stop();r.WriteFlushMs=timer.Elapsed.TotalMilliseconds;CloseHandle(h);h=new IntPtr(-1);
   h=Open(path,0x80000000,0x20000000);Check(SetFilePointerEx(h,offset,out at,0),"Seek read");timer.Restart();
   for(int i=0;i<read.Length;i+=8192) {Check(ReadFile(h,buffer,8192,out done,IntPtr.Zero),"Read chunk "+(i/8192));Check(done==8192,"Short read");Marshal.Copy(buffer,read,i,8192);r.ReadBytes+=(int)done;}
   timer.Stop();r.ReadMs=timer.Elapsed.TotalMilliseconds; File.WriteAllBytes(dump,read);
   for(int i=0;i<read.Length;i++) if(read[i]!=payload[i]){if(r.FirstMismatch<0)r.FirstMismatch=i;r.Mismatches++;}
   using(var sha=System.Security.Cryptography.SHA256.Create()) r.ReadSHA256=BitConverter.ToString(sha.ComputeHash(read)).Replace("-","").ToLowerInvariant();
   return r;
  } finally {if(h!=new IntPtr(-1))CloseHandle(h);VirtualFree(buffer,UIntPtr.Zero,0x8000);}
 }
}
