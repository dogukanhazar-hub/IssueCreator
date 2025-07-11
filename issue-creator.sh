#!/bin/bash

TOKEN="EnterYourTokenHere"

TITLE=""
BODY=""
OWNER=""
REPO=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --title)
      TITLE="$2"
      shift 2
      ;;
    --description)
      BODY="$2"
      shift 2
      ;;
    --owner)
      OWNER="$2"
      shift 2
      ;;
    --repository)
      REPO="$2"
      shift 2
      ;;
    *)
      echo "Unknown parameter: $1"
      exit 1
      ;;
  esac
done

if [[ -z "$TITLE" || -z "$BODY" || -z "$OWNER" || -z "$REPO" ]]; then
  echo "Usage: issue-creator --title \"title\" --description \"description\" --owner \"username\" --repository \"repo\""
  exit 1
fi

json_data=$(jq -n \
  --arg token "$TOKEN" \
  --arg repo "$REPO" \
  --arg title "$TITLE" \
  --arg body "$BODY" \
  '{access_token: $token, repo: $repo, title: $title, body: $body}')

curl -s -X POST "https://gitee.com/api/v5/repos/$OWNER/issues" \
  -H "Content-Type: application/json" \
  -d "$json_data" | jq '.html_url'
