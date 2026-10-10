#!/bin/bash
set -o pipefail
export DOTNET_CLI_TELEMETRY_OPTOUT=1
export DOTNET_NOLOGO=1
export DOTNET_CLI_HOME="$PWD/dotnet-home"
export NUGET_PACKAGES="$PWD/nuget-packages"
run() { printf '\nCOMMAND: '; printf '%q ' "$@"; printf '\n'; "$@"; rc=$?; echo "EXIT_CODE=$rc"; if [ "$rc" -ne 0 ]; then exit "$rc"; fi; }
run command -v dotnet
run dotnet --info
run dotnet --list-sdks
run dotnet --list-runtimes
run dotnet new console --name T1000CSharp --output csharp --framework net10.0 --no-restore
cat > csharp/Program.cs <<'EOF'
using System;
using System.Linq;
using System.Runtime.InteropServices;
int sum = new[] {10, 12, 20}.Sum();
Console.WriteLine($"T1000 C#: sum={sum}; runtime={RuntimeInformation.FrameworkDescription}; os={RuntimeInformation.OSDescription}");
if (sum != 42 || !OperatingSystem.IsLinux()) throw new Exception("Qualification failed");
Console.WriteLine("T1000 C# VALIDATION OK");
EOF
run dotnet restore csharp/T1000CSharp.csproj
run dotnet build csharp/T1000CSharp.csproj --configuration Release --no-restore
run dotnet run --project csharp/T1000CSharp.csproj --configuration Release --no-build
run dpkg --audit
