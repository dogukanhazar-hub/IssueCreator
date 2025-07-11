# FastIssue

#### Description
{**This Bash script sends a POST request to the Gitee API to create a new issue in the specified repository. It accepts parameters via command-line arguments and requires a Gitee personal access token for authentication.

Features:

Easy to use with simple CLI arguments.

Supports creating issues with title and description.

Works directly with Gitee API.

Lightweight and requires minimal setup.**}

#### Software Architecture
Software architecture description

#### Installation

To install and prepare the issue-creator script for use:

1. Download or clone the script:
git clone https://gitee.com/yourusername/FastIssue.git
cd FastIssue

2. Make the script executable:
chmod +x issue-creator.sh

3. (Optional) Move the script to a directory in your system PATH:
sudo mv issue-creator.sh /usr/local/bin/issue-creator

4. Install required dependency:
The script uses jq to parse JSON. Install it using:
sudo apt update
sudo apt install jq

#### Instructions

1. Set your Gitee Personal Access Token:
TOKEN="xxxPUT_YOUR_GITEE_TOKEN_HERExxx" in the issue-creator.sh
2. Run the script with the following arguments:
./issue-creator.sh --title "Issue Title" --description "Issue Description" --owner "your-gitee-username" --repository "your-repo-name"
3. If you moved the script to a system path, you can call it directly as:
issue-creator --title "Bug in login" --description "Login button does not respond" --owner "your-gitee-username" --repository "FastIssue"
4. After execution, the script will print the URL of the newly created issue if successful.

