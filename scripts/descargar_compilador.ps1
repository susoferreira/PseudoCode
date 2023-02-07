

# Source URL
$url = "https://github.com/skeeto/w64devkit/releases/download/v1.17.0/w64devkit-1.17.0.zip"
# Destation file
$dest = "compiler.zip"
# Download the file
Invoke-WebRequest -Uri $url -OutFile $dest

Expand-Archive "compiler.zip" -DestinationPath .
remove-item "compiler.zip"