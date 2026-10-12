# Pull Request and Commit Guide

This guide collects the cFE conventions for preparing commits and pull requests.
It complements the top-level
[cFS Contributing Guide](https://github.com/nasa/cFS/blob/main/CONTRIBUTING.md);
the top-level guide remains authoritative for general contribution requirements.

## Before starting

1. Find the issue that describes the work, or open one before implementing a
   change.
2. Start from the current `dev` branch.
3. Create a focused branch. The cFS contributing guide recommends names in the
   form `fix-ISSUE_NUMBER-<FIX_SUMMARY>`.
4. Keep the pull request limited to the linked issue. Unrelated cleanup should
   be submitted separately.
5. Draft pull requests are encouraged when early feedback would prevent
   unnecessary rework.

## Commit messages

Use the cFS commit subject convention:

```text
Fix #XYZ, SHORT_DESCRIPTION

LONG_DESCRIPTION (optional)
```

Write commit messages so that the repository history explains the reason for a
change, not only the diff.

For the subject line:

- keep it concise;
- use the imperative form;
- capitalize the first word;
- do not end it with a period;
- separate it from an optional body with a blank line.

When a body is useful, explain the problem, the reason for the change, relevant
tradeoffs, and any non-obvious consequences. Avoid repeating implementation
details that are already clear from the diff.

These recommendations are consistent with the commit-message practices
referenced in issue #2788:
[How to Write a Git Commit Message](https://cbea.ms/git-commit/).

## Commit scope and history

Each commit should represent one logical topic. A pull request may contain more
than one commit when the commits are independently meaningful, but unrelated
topics should not be combined into a single pull request.

During draft review it is reasonable to commit incremental changes so reviewers
can see progress. Before the pull request is ready to merge:

- amend or squash fixup-only commits where appropriate;
- remove accidental or unrelated changes;
- make sure each remaining commit has a useful message;
- make sure the complete series builds and tests as required by the selected
  pull request template.

Prepare the branch as if its individual commits will remain visible in project
history. If reviewers request a different commit layout, follow that review
direction before merge.

## Opening the pull request

Target `dev` unless the issue or a maintainer specifies another branch.

Use the project pull request title convention:

```text
Fix #XYZ, SHORT_DESCRIPTION
```

Select the template that matches the change and provide the requested evidence.
At minimum, make the following clear:

- the linked issue;
- what changed and why;
- testing or documentation validation performed;
- expected behavior and requirements impact when applicable;
- the areas of expertise touched by the change.

For changes that affect more than one cFS repository, keep each repository's
pull request independently reviewable and cross-reference the related issue or
pull requests.

## Updating a pull request

Respond to review with focused changes. If a review change only corrects the
current logical commit, amend or squash it before final merge rather than
leaving a chain of temporary `fixup` commits.

Do not rewrite reviewed history unnecessarily. When a rebase or force-push is
needed, make the resulting history easier to review, not harder.

## Merge readiness

A pull request is ready for maintainer merge when:

- the linked issue and scope are clear;
- the selected template is complete;
- required tests and workflows have passed, or any expected exception is
  documented;
- review feedback has been addressed;
- the commit history is organized into meaningful logical changes;
- documentation and links introduced by the pull request have been checked.

Maintainers make the final merge decision and may request additional history
cleanup or validation before accepting the change.
