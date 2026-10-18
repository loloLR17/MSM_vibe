from pathlib import Path
import subprocess
p=Path('/home/lolo/dev/msm/reports/TR2-r5b2');s=Path('/home/lolo/dev/msm/reports/TR2-r5afix/BenchMaster.cs').read_text();s=s.replace('ONE genuine START5/10 and STOP5/11 only','separate explicit START5/12/run11 or STOP5/13; never two masters');a=s.index('  string mode=args[0];');b=s.index('  port=new SerialPort',a);s=s[:a]+'''  string mode=args[0];startTx=12;stopTx=13;
  if(mode=="exclusive"){
   try { using(var check=new SerialPort("COM4",115200,Parity.Even,8,StopBits.One)){check.Open();} }
   catch(UnauthorizedAccessException){Log(new{kind="exclusive_owner_confirmed",utc=DateTime.UtcNow.ToString("o"),port="COM4",passed=true,no_transmission=true});return 0;}
   throw new IOException("Port unexpectedly available; no exclusive live owner confirmed; no transmission.");
  }
  if(mode!="start"&&mode!="stop")throw new ArgumentException("start or stop explicit one-shot command only");
'''+s[b:];a=s.index('  if(mode=="ro"){');b=s.index('  Log(new{kind="summary"',a);s=s[:a]+'''  ushort[] initial=Read(3000,48);
  if(mode=="start"){
   if(initial[0]!=1||(initial[1]&6)!=0)throw new IOException("Not idle before START; no write.");
   Command(startTx,13);Wait(startTx,13,false);
   if(valid==0)throw new IOException("No authoritative physical window");
  }else{
   if((initial[1]&6)!=6)throw new IOException("Not valid/fresh before STOP; no write.");
   Command(stopTx,14);Wait(stopTx,14,false);
   ushort[] b=Read(3000,48);if((b[1]&4)!=0||(b[1]&512)==0||b[0]!=1)throw new IOException("STOP/HELD not observed");
  }
'''+s[b:];(p/'BenchMaster.cs').write_text(s)
win=lambda x:subprocess.check_output(['wslpath','-w',str(x)],text=True).strip()
with (p/'master-build.log').open('wb') as log:subprocess.run(['/mnt/c/Windows/Microsoft.NET/Framework64/v4.0.30319/csc.exe','/nologo','/r:System.Web.Extensions.dll','/out:'+win(p/'BenchMaster.exe'),win(p/'BenchMaster.cs')],stdout=log,stderr=subprocess.STDOUT,check=True)
(p/'BenchMaster.exe').chmod(0o744)
with (p/'master-selftest.jsonl').open('wb') as log:subprocess.run([str(p/'BenchMaster.exe'),'selftest'],stdout=log,stderr=subprocess.STDOUT,check=True,timeout=25)
