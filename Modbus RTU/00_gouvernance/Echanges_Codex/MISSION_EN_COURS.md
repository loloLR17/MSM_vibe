---
mission_id: TR2-20261013-A04-01-SAFE-BUILD
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A04-01 — Sécurisation des profils de compilation STM32

## Objectif
Corriger uniquement la dette A04-01 : éliminer le risque de confusion entre le firmware cumulatif M014/M015 et le harness générique susceptible d'écrire sur la microSD. Conserver les usages historiques légitimes derrière une sélection explicite. Ne pas modifier les fonctionnalités FRAM/Modbus/FIFO ni activer SDMMC dans le cumulatif.

## Références
Consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, PROTOCOLE_ECHANGE_V2.md, DETTE_TECHNIQUE_A04.md, rapports archivés A-04/A-05 et matrice A-05 proposed-test-matrix.csv sur main. Vérifier le HEAD courant.

## Travaux
Analyser CMake, scripts de build et de validation, main.c, chemins SDMMC et procédures de programmation. Mettre en place la correction minimale de sélection sûre : profil cumulatif explicite et reproductible, identification claire des artefacts, garde contre profil générique ambigu et accès SD non intentionnels. Préserver les profils spécialisés sans les présenter comme firmware de production. Pas de refonte générale.

## Acceptation
- Exécuter les scénarios A-05 BUILD-01, BUILD-02, BUILD-03 et SD-01, y compris les gardes négatives déjà établies.
- Vérifier les chemins de code et artefacts ELF, pas uniquement l'absence d'un symbole SD.
- Reconstruire le cumulatif canonique A-04 et, si le runtime est inchangé, vérifier le BIN identique au SHA256 8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480 (169380 octets).
- Exécuter les tests logiciels pertinents et six tests Python d'autorité ; ajouter seulement des non-régressions ciblées. Publier commandes, résultats et preuves.
- Ne pas accéder au matériel STM32 ni à la microSD : aucun flash, debug ou test destructif.

## Autonomie et limites
Commit/push autorisés sur main pour code et preuves, conformément à AGENTS.md. Flash/debug désactivés pour cette tranche logicielle. Préserver les acquis M014/M015, worktrees et stashes. Ne pas corriger A04-02 ou A04-03. Escalader uniquement les décisions structurantes ou risques non maîtrisés.

## Livrable
Archiver le rapport A-05 byte-identique sous Echanges_Codex/archives/TR2-20261013-A05-AUDIT-TESTS/RAPPORT.md s'il est absent. Publier DERNIER_RAPPORT.md avec mission_id, statut, commits, diff, tests, empreintes et limites ; vérifier les publications distantes. Ne pas clore la dette A04-01 : ChatGPT vérifiera les résultats et mettra le registre à jour. Reprise sous le même identifiant en cas d'interruption.
