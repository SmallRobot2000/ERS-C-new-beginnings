# Get all .c files in the lib directory
$files = Get-ChildItem -Path "lib" -Filter "*.c"

# Loop through each file and execute the command
foreach ($file in $files) {
    $filename = [System.IO.Path]::GetFileNameWithoutExtension($file.Name)
    $command = "cc65 -O --cpu 65sc02 -Oi -t none `"$($file.FullName)`" -o `"bin\$filename.asm`" -I include"
    #echo $command
    $output = $output + "`n" + (Invoke-Expression $command)
    
    #echo $($file.FullName)
}

# Get all .c files in the src directory
$files = Get-ChildItem -Path "src" -Filter "*.c"

# Loop through each file and execute the command
foreach ($file in $files) {
    $filename = [System.IO.Path]::GetFileNameWithoutExtension($file.Name)
    $command = "cc65 -O --cpu 65sc02 -Oi -t none `"$($file.FullName)`" -o `"bin\$filename.asm`" -I include"
    #echo $command
    $output = $output + "`n" + (Invoke-Expression $command)
    
    #echo $($file.FullName)
}


# Get all .asm files in the lib directory
$asmFiles = Get-ChildItem -Path "lib" -Filter "*.asm"

# Copy each .asm file to the destination directory, overwriting if necessary
foreach ($file in $asmFiles) {
    Copy-Item -Path $file.FullName -Destination bin -Force
}



# Get all .o files in the source directory and move them to the destination directory
Get-ChildItem -Path bin -Filter *.asm | Move-Item -Destination bin\asm -Force



# Get all .c files in the src directory
$files = Get-ChildItem -Path "bin\asm" -Filter "*.asm"

# Loop through each file and execute the command
foreach ($file in $files) {
    $filename = [System.IO.Path]::GetFileNameWithoutExtension($file.Name)
    $command = "ca65 --cpu 65sc02 `"$($file.FullName)`" -o `"bin\$filename.o`""
    #echo $command
    $output = $output + "`n" + (Invoke-Expression $command)
    
    #echo $($file.FullName)
}


# Get all .o files in the source directory and move them to the destination directory
Get-ChildItem -Path bin -Filter *.o | Move-Item -Destination bin\out -Force

# Define the configuration file and output file
$configFile = "cfg/ERS.cfg"
$outputFile = "bin/PROGRAM.out"
$libraryFile = "lib/none.lib"

# Get all .o files in the bin directory
$oFiles = Get-ChildItem -Path "bin\out" -Filter "*.o"

# Create a string with all .o files in sequence
$oFilesString = ""
foreach ($file in $oFiles) {
    $oFilesString += "`"$($file.FullName)`" "
}

# Construct the ld65 command
$command = "ld65 -C $configFile $oFilesString $libraryFile -o $outputFile"

# Execute the command
#Write-Host "Executing: $command"
$output = $output + "`n" + (Invoke-Expression $command)

# Define the path to the executable and the arguments
$exePath = "fileFormater.exe"
$args = "bin/PROGRAM.out", "bin/PROGRAM.ers"

# Run the executable with the arguments
$process = Start-Process -FilePath $exePath -ArgumentList $args -NoNewWindow -RedirectStandardOutput "output.txt" -PassThru

# Wait for the process to exit
$process.WaitForExit()

# Read and print the output
$output = $output + "`n" +(Get-Content "output.txt" -Raw)


# Remove empty lines
$output = $output -split "`n" | Where-Object { $_.Trim() -ne "" } | Out-String

echo $output

# Delete "output.txt"
Remove-Item -Path "output.txt"
