---
mission_id: TR2-20261011-STLINK-009
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 009 - Diagnostic ST-LINK autonome

L'utilisateur demande a Codex de prendre en charge le diagnostic ST-LINK pour eviter les copier-coller. STM32CubeProgrammer sous Windows affiche "No debug probe detected". OpenOCD sous WSL a ete arrete par l'utilisateur. La sonde pourrait rester attachee a WSL via usbipd.

Lire AGENTS.md, le protocole de gouvernance, le rapport de mission 008 et les procedures T1000. Verifier la branche main, le HEAD et les travaux existants. Diagnostiquer directement la visibilite de la sonde depuis Windows et WSL, usbipd et les processus concernes. Utiliser les outils hote accessibles depuis WSL si autorises. Ne pas demander de copier-coller pour des operations que Codex peut effectuer. Si des droits administrateur ou une intervention physique sont necessaires, fournir une unique instruction precise.

Ne pas flasher ni debugger dans cette mission de diagnostic. Preparer la sauvegarde du firmware existant, puis la procedure de flash BIN008 et essai RX COM7 pour une mission ulterieure. BIN008 SHA256: 02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b. ST-LINK attendu: 003500463235510B37333439.

Autorisation de documentation, tests logiciels non destructifs, commit et push normaux sur main. Interdiction de flash, debug cible, emission COM7, manipulation physique, operations Git destructives et force push.

Publier DERNIER_RAPPORT.md mission009 avec preuves et limitations reelles. Verifier la publication distante.
