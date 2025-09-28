# ----------------------------
# Single-threaded Redis Benchmark with Percentile Storage
# ----------------------------

$server = "127.0.0.1"
$port = 3000

# Batch sizes
$batches = @(10, 100, 1000, 10000, 100000)

# Commands to test
$commandsToTest = @(
    @{ Name="PING"; Build={ "*1`r`n$4`r`nPING`r`n" } },
    @{ Name="SET";  Build={ param($i) "*3`r`n$3`r`nSET`r`n$5`r`nkey$i`r`n$5`r`nval$i`r`n" } },
    @{ Name="GET";  Build={ param($i) "*2`r`n$3`r`nGET`r`n$5`r`nkey$i`r`n" } }
)

# Helper to compute percentiles
function Get-Percentile($data, $percent) {
    $sorted = $data | Sort-Object
    $index = [math]::Floor($sorted.Count * $percent)
    return $sorted[$index]
}

# Store all results
$allResults = @()

foreach ($batchSize in $batches) {
    foreach ($cmd in $commandsToTest) {
        $latencies = @()

        $tcp = New-Object System.Net.Sockets.TcpClient($server, $port)
        $stream = $tcp.GetStream()

        for ($i = 1; $i -le $batchSize; $i++) {
            $msg = if ($cmd.Name -eq "PING") { $cmd.Build.Invoke() } else { $cmd.Build.Invoke($i) }
            $bytes = [System.Text.Encoding]::UTF8.GetBytes($msg)

            $sw = [System.Diagnostics.Stopwatch]::StartNew()
            $stream.Write($bytes, 0, $bytes.Length)

            $buffer = New-Object byte[] 1024
            $stream.Read($buffer, 0, $buffer.Length)
            $sw.Stop()

            $latencies += $sw.Elapsed.TotalMilliseconds
        }

        $stream.Close()
        $tcp.Close()

        # Compute stats
        $avg = ($latencies | Measure-Object -Average).Average
        $p50 = Get-Percentile $latencies 0.5
        $p95 = Get-Percentile $latencies 0.95
        $p99 = Get-Percentile $latencies 0.99

        # Save result
        $allResults += [PSCustomObject]@{
            BatchSize = $batchSize
            Command   = $cmd.Name
            Avg       = $avg
            P50       = $p50
            P95       = $p95
            P99       = $p99
        }

        Write-Output "Completed: $($cmd.Name) for batch $batchSize"
    }
}

# ----------------------------
# Display all percentile results separately
# ----------------------------
Write-Output "`n==== Benchmark Summary ===="

foreach ($result in $allResults) {
    Write-Output "Command: $($result.Command), Batch: $($result.BatchSize)"
    Write-Output ("Avg latency: {0:N6} ms" -f $result.Avg)
    Write-Output ("P50 latency: {0:N6} ms" -f $result.P50)
    Write-Output ("P95 latency: {0:N6} ms" -f $result.P95)
    Write-Output ("P99 latency: {0:N6} ms" -f $result.P99)
    Write-Output ""
}

# Optionally: export to CSV for further analysis
# $allResults | Export-Csv "benchmark_results.csv" -NoTypeInformation
