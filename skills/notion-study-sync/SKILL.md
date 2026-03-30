---
name: notion-study-sync
description: Use this skill when working in the Linux driver study repository and you need to turn a user question plus the assistant answer into a clean Notion study note entry with stable field mapping for Question, Created, Summary, and page body.
---

# Notion Study Sync

Use this skill when the task is to organize Linux driver study Q&A into the repository's Notion notes database.

The target database is:

- `https://www.notion.so/3fc2551a8fd5832fa33b8130c3f6e385?v=4152551a8fd582b6ba34882881cccb57&source=copy_link`

## What to write

For each user question, create one new Notion page.

Map content like this:

- `Question`: the user's original question text
- `Created`: the time the question was asked
- `Summary`: a concise summary within 15 Chinese characters
- Page body: the assistant's answer

## Writing rules

- Keep `Question` close to the user's original wording.
- Use a short `Summary` that helps later retrieval.
- Keep the page body technically accurate and readable as study notes.
- Include repo file paths in the page body when they help future review.
- Do not mention successful syncing in the user-facing answer unless the user explicitly asks.
- If syncing fails, briefly tell the user and continue the main task.

## Recommended summary style

Prefer short noun phrases such as:

- `字符设备初始化`
- `Makefile双阶段`
- `globalmem设备号`
- `reinsmod作用`
- `missing prototype警告`

## When not to use

Do not use this skill for:

- general coding work that does not need Notion recording
- long transcript dumping without summarization
- tasks outside this repository's Linux driver study workflow
