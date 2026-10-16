---
mission_id: TR2-20261016-R4P-PHYSICAL-COEXISTENCE
status: READY
base_ref: main
base_sha: f7366e535b90a4ef79642a3debfa601f35c40b9c
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-P — Reprise contrôlée de la qualification physique FRAM/FIFO

## Situation et objectif
R4-D est DONE pour son diagnostic : START et OBSERVE consécutifs dépassaient 1000us ; trois checkpoints ont été ajoutés au moteur R2 (commit 7a6a8b924f4e616adfcb5fb2dc52c7ccc16297e0). Probe physique corrigé : workmax911us, mais sans admission R2 ni FRAM en charge. La FRAM de TR2-001 conserve STARTED epoch2/run2 indéterminé9 ; Flash initiale M016 restaurée, FRAM complète SHA256 f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1. R4/R6 non qualifiés.

Objectif : reprendre, par étapes et preuves, la qualification physique du chemin R2 avec écritures FRAM et acquisition FIFO simultanées, sans sacrifier l'intégrité historique, l'anti-rejeu, les budgets ni l'arrêt sûr. Ne pas élargir prématurément la mission à R5/R6, RTU électrique ou SD.

## Préparation et protection
Lire sur main AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, ETAT_COURANT_TR2, roadmap, R0 (migration FRAM et journal), LIVE_R1/R2/R3/R4/R4M/R4D, derniers rapports et preuves. Vérifier HEAD, archiver byte-identiques mission/rapport R4-D et conserver stashes/worktrees. Identifier T1000, TR2-001/UID/ST-LINK et capteur ; recontrôler Flash/FRAM double lecture et sauvegardes intégrales récupérables avant flash ou écriture ; confirmer arrêt capteur, allocations LIVE [122880,245760), historique [0,122880), tail [245760,262144). Si l'état réel diffère des empreintes attendues, arrêter toute écriture et analyser.

## Exécution progressive
1. Analyser le verrou STARTED epoch2/run2 et les vrais reçus/INTENT. Aucun résultat rétroactif inventé, aucune réadmission/rejeu d'une clé consommée. Construire et tester hors cible la transition de maintenance strictement bornée vers un nouveau contexte de qualification, en réutilisant autant que possible les outils R4-M et le vrai moteur RENEW, sans effacement brut ni modification de R0. Conserver le snapshot original intégral hors cible et vérifier la monotonie epoch/run. Si la sûreté de la transition n'est pas démontrable, rester en diagnostic, documenter BLOCKED et ne pas écrire sur FRAM.
2. Si les préconditions et preuves de sécurité sont satisfaites, autoriser sur le seul banc TR2-001 une isolation logique unique et traçable du journal LIVE puis RENEW authentique vers une nouvelle epoch, selon procédure qualifiée. Cette autorisation couvre uniquement la partition LIVE et ne transforme pas le verrou précédent en COMPLETED. Interdire mass erase, reprovisionnement aveugle, réinitialisation de l'identité, réemploi des anciennes clés, modification historique/tail, Option Bytes/RDP et toute écriture hors LIVE. Journal d'adresses et lecture retour ; arrêt si doute ou nouveau STARTED indéterminé.
3. Vérifier la correction R4-D dans le vrai moteur R2 en charge avec quanta FRAM/CRC, acquisition FIFO et observation capteur ; mesurer DWT/HCLK, workmax<=1000us, servicemax<=2500us, gap FIFO<=5ms en régime continu (premier mot distinct), niveaux FIFO, pertes/timestamps, délais STOP, erreurs et CRC. Décomposer les mesures ; ne pas confondre nominal et WCET en faute. Ne pas relâcher les seuils pour obtenir PASS. Préserver les barrières transactionnelles et la non-réentrée.
4. Si START et le journal sont durablement terminalisés sans ambiguïté, poursuivre progressivement les scénarios prévus R4 : RUN prolongé (objectif 60s), STOP métier, APPLY, START suivant, injection de faute de service et recovery lecture seule après reset, avec identité et preuves à chaque étape. Arrêter au premier état indéterminé, perte d'échantillon non signalée, dépassement non maîtrisé ou anomalie matérielle. Ne pas multiplier les tentatives ni nettoyer un nouveau verrou pour obtenir PASS.
5. La coexistence Modbus/UART et l'impact des lectures d'images RAM sont un risque identifié mais non une condition préalable à cette tranche FRAM/FIFO : conserver les tests/modèles existants, noter les besoins de mesures IRQ/TX et reporter la qualification RTU électrique à R6. Pas de serveur PC/SQLite/Web ni microSD dans R4-P.
6. Tester et publier les corrections minimales uniquement si défaut démontré : tests hôte, coupures/recovery, ASan/UBSan ciblés et cross-build ARM, binaire V1 non régressé. Préserver firmware initial par restauration et comparaison intégrale Flash, état FRAM final exact avec sauvegardes et empreintes ; ne jamais restaurer FRAM par défaut en écrasant les preuves.

## Acceptation et conditions d'arrêt
Rapport indépendant des suppositions : identité matérielle, SHA code/ELF/BIN réellement flashé, provenance toolchain, mesures physiques de FIFO/FRAM/R2, journal et anti-rejeu, arrêt/STOP et recovery, limites et défauts résiduels. Si coexistence non démontrée, status BLOCKED, aucune qualification R4/R6. Le succès des tests hôte n'est pas une preuve physique. Un nouveau STARTED indéterminé, risque matériel, changement d'identité ou écart de sauvegarde impose l'arrêt de l'écriture et l'archivage.

## Autonomie et livrables
Autorisations explicites : allow_commit true, allow_push true, allow_flash true, allow_debug true, target_branch main. Garde-fous AGENTS.md applicables ; interventions physiques seulement par l'utilisateur si indispensables. Codex conduit l'analyse, les essais et les corrections proportionnées, publie code, preuves, ETAT_COURANT_TR2/roadmap factuels et DERNIER_RAPPORT.md, vérifie publication distante. ChatGPT effectuera la revue indépendante ; pas de qualification automatique ni de tag prématuré.
