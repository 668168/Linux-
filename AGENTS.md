# Repository Guidelines

## Repository Purpose
This repository is primarily a study workspace for learning Linux device driver code. It combines reading notes, runnable kernel module examples, small user-space test programs, and reference material from multiple sources.

Any change made in this repository should support learning one or more of the following:

- understanding driver architecture and kernel interfaces
- tracing how example code works
- comparing different implementation styles across chapters or sources
- improving notes, explanations, and experiment reproducibility

When adding or editing content, prefer clarity and educational value over broad refactors or cosmetic cleanup.

## Project Structure & Module Organization
Top-level content is organized by purpose:

- `docs/`: study notes, summaries, and concept explanations
- `code/`: main driver example tree, usually grouped by chapter number such as `code/06/` and `code/18/`
- `simple/`: smaller companion samples and book snapshot examples
- `实例代码/`: additional Chinese-language driver and experiment examples
- `image/` and `reference/`: screenshots and reference material

Prefer adding new study notes under `docs/`. For code changes, place files beside the closest chapter or example directory instead of creating new top-level folders.

## Preferred Contributions
Useful changes in this repository include:

- adding explanations for tricky driver code paths
- annotating relationships between kernel APIs, data structures, and callbacks
- adding concise build/run notes for specific examples
- improving experiment steps for loading, testing, and unloading modules
- adding comparison notes between similar samples
- fixing code or docs when the fix improves learning accuracy

Avoid large restructures that make the original example layout harder to map back to the source material.

## Build, Test, and Development Commands
There is no single repo-wide build. Most code samples are built inside their own directory with a local `Makefile`.

- `make modules`: build a kernel module in the current sample directory
- `make clean`: remove generated module build artifacts
- `make insmod` / `make rmmod`: load or unload sample modules when provided
- `make ioctl_test` or `make test_ioctl`: build and run sample user-space tests in directories that define them

Example:
```sh
cd code/06/包含2个globalmem设备的驱动
make modules
make ioctl_test
```

When touching code examples, prefer validating the specific sample locally instead of attempting repo-wide commands.

## Coding Style & Naming Conventions
Follow the surrounding file style instead of reformatting aggressively.

- In C sources, keep kernel-style formatting consistent with nearby files.
- Use tabs where the existing Makefiles or sample code use tabs.
- Keep brace placement and macro style aligned with the local example.
- Prefer descriptive snake_case names such as `globalmem_ioctl_test.c`.
- Keep chapter and example directory names unchanged; many paths reflect book structure and learning context.

For Markdown:

- use short sections and concise explanations
- prefer content that explains why code exists, not just what it does
- use relative links such as `docs/Linux内核模块.md`

## Testing Guidelines
Testing is example-specific, not centralized.

- Build the target example locally before submitting code changes.
- Run any provided user-space test binary when relevant.
- If the sample supports it, verify at least one load/unload cycle.
- Record assumptions when a sample depends on a specific kernel version or header layout.

Do not commit generated artifacts such as `.o`, `.ko`, `.mod.c`, `modules.order`, temporary binaries, or ad hoc output files.

## Documentation Expectations
Because this repository is used for study, documentation changes should help future reading and review.

- Explain key kernel entry points, file operations, and data flow.
- Call out version-sensitive behavior when known.
- Distinguish clearly between copied example code and your own interpretation.
- Prefer small, local explanations near the relevant example rather than long disconnected notes.

## Notion Sync Workflow
This repository uses a Notion database as the default place to collect study Q&A and any question i asked.

- Default Notion database:
  `https://www.notion.so/3fc2551a8fd5832fa33b8130c3f6e385?v=4152551a8fd582b6ba34882881cccb57&source=copy_link`
- When the user asks a question in this repository and receives an answer, treat syncing that Q&A to the Notion database as the default follow-up behavior.
- Unless the user says otherwise, create one new Notion page for each user question.
- Put the user's original question into the Notion database property `Question`.
- Put the user's question time into the Notion database property `Created`.
- Put a concise less than 15 keywords into the Notion database property `Summary`.
- Put the assistant's answer into the Notion page body.

Each synced entry should use a consistent structure:

- Database properties:
  - `Question`: the user's original question, kept as close to the original wording as possible
  - `Created`: the local date and time when the user asked the question
  - `Summary`: a keyword summary within 15 Chinese characters
- Page body:
  - the assistant's answer
  - include relevant repo paths when they materially help later review
  - keep the body readable as a study note, but do not remove technical detail that changes meaning

Syncing rules:

- Preserve technical accuracy over verbatim transcript style.
- Preserve the user's original question text in `Question`; only make minimal cleanup if formatting would break the field.
- Create a new page for every new user question instead of appending to a daily page.
- Treat Notion sync as a required end-of-turn step for every answered user question in this repository.
- Complete the Notion write before finishing the turn whenever the target database is available.
- Run Notion syncing silently by default.
- Do not mention successful Notion syncing in normal answers unless the user explicitly asks about it.
- If the target database does not have `Question`, `Created`, or `Summary`, tell the user briefly and continue answering normally.
- If Notion is unavailable or the target cannot be written, tell the user briefly and continue answering normally.

after Syncing:
 - if succeeded , do not notify me you have sync to notion.
 - if fail, notify me.
 
## Commit Guidelines
Use short, imperative commit messages such as:

- `add notes for platform driver example`
- `rewrite globalmem_two ioctl interface`
- `clarify char device registration flow`

Keep each commit focused on one logical learning-oriented change.

## Security & Configuration Tips
Many samples assume local kernel headers and may require `sudo` for `insmod`, `rmmod`, and device-node creation.

- Review Makefiles before running privileged targets.
- Be cautious with older examples that hardcode legacy kernel paths or assumptions.
- Prefer understanding what a module does before loading it.
- If a command modifies device nodes or kernel state, document the effect in notes when useful for future study.
