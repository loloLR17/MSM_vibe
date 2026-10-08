---
mission_id: TR2-20261008-CODEX-V2B1-001
status: READY
base_ref: test/echange-chatgpt-codex-20261008
base_sha: 3245abec29b1a3aa3a307c681a7aa029348f4567
created_at_utc: 2026-10-08
author: ChatGPT
---

# V2-B1 — Qualification comparative des sessions Codex

## Objectif
Comparer le comportement réel de Codex sous quatre couples sandbox / approbations, **sans changer la configuration permanente**, sans intervention matérielle et sans modifier le firmware. Cette mission est une campagne d'observation, **pas** une autorisation d'adopter `danger-full-access`.

## Préalables et périmètre
Lire `AGENTS.md`, `PROTOCOLE_ECHANGE_V2.md` et `PROPOSITION_GOUVERNANCE_V2_B.md`. Vérifier la référence distante et l'état Git local ; préserver intégralement les travaux préexistants. Ne jamais tenter de corriger le dépôt sale par `reset`, `clean`, `restore`, `checkout --`, merge ou rebase.

Autorisé : commandes de lecture Git, consultation distante, création/lecture de fichiers temporaires sous `/tmp/tr2-v2b1/`, lancement de processus interactifs **non matériels**, diagnostic `--help` d'outils si sans effet de bord, et production du rapport. Aucun build requis.

Interdit : flash, programmation, effacement, reset de cible, GDB connecté au ST-LINK, modification du firmware, de `AGENTS.md`, des règles ou de `~/.codex/config.toml`, accès aux secrets/jetons, `push` sur `main`, changement permanent de sandbox. **Ne pas contourner une demande d'approbation refusée**.

## Matrice — sessions indépendantes
Une session Codex doit être lancée séparément pour chaque configuration :

- C1 : `--sandbox workspace-write --ask-for-approval on-request`
- C2 : `--sandbox workspace-write --ask-for-approval never`
- C3 : `--sandbox danger-full-access --ask-for-approval on-request`
- C4 : `--sandbox danger-full-access --ask-for-approval never`

Pour chaque session : relever version Codex, paramètres effectivement demandés, statut Git, succès/échec et codes des opérations non destructives ci-dessous, nombre et motif des demandes d'approbation. **Ne pas inférer la configuration effective à partir de la seule ligne de commande si une preuve supplémentaire est nécessaire.**

### Batterie d'essais par session
1. `git status --short --branch`, `git rev-parse HEAD`, `git ls-remote origin refs/heads/main refs/heads/test/echange-chatgpt-codex-20261008` ; noter distinctement erreurs Git, réseau et approbation.
2. Dans un répertoire temporaire isolé propre à la configuration : tenter un `git clone --single-branch --branch test/echange-chatgpt-codex-20261008` ; lire la mission ; **ne pas pousser** pendant les essais C1/C2/C4.
3. Démarrer un processus Python interactif temporaire qui imprime READY puis répond PONG à une entrée ; vérifier la capacité à lui transmettre une deuxième commande dans la même session. Ne pas lancer de processus persistant sans fin.
4. Si disponibles, vérifier seulement `STM32_Programmer_CLI.exe --help`, `gdb-multiarch --version` et `ST-LINK_gdbserver.exe --help` ; pas de connexion au MCU, aucun argument de programmation. Ne pas répéter un diagnostic qui déclenche une opération matérielle ou une demande d'approbation non autorisée.
5. Documenter les blocages de sandbox, DNS, WSL/vsock et approbation, sans généraliser un échec intermittent.

### Publication unique, après comparaison
Le **seul fichier du dépôt que Codex peut modifier** est `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md`, et **uniquement dans la session C3**, après exécution de C1, C2, C3, C4 et récupération de leurs observations temporaires. Autorisation explicite de commit et push normaux **sur la branche expérimentale seulement**, après contrôle du diff et relecture distante. Si C3 ne permet pas de publier, déclarer `BLOCKED` ; demander une décision humaine, sans élargir les permissions ni publier depuis C4.

Le rapport inclut : `mission_id: TR2-20261008-CODEX-V2B1-001`, statut `DONE` seulement après vérification distante, `base_ref`, `base_sha`, `initial_head`, `result_sha: null`, auteur, date, matrice des quatre configurations, résultats détaillés et codes, demandes d'approbation, incidents, comparaison des risques et **recommandation argumentée non appliquée**. Ne jamais écrire de secrets ou journaux bruts sensibles dans GitHub public.

## Conditions d'arrêt
Si une opération peut modifier le matériel, le firmware, la configuration permanente ou des travaux locaux préexistants : arrêter cette opération. Si les sessions ne peuvent pas échanger leurs observations par fichiers temporaires, rapporter la limitation ; ne pas inventer de résultats. Toute modification de configuration permanente requiert une décision ultérieure de l'utilisateur.

## Critères de réussite
Les quatre sessions sont lancées indépendamment ; chaque résultat est attribué à la bonne configuration ; un rapport unique est publié et relu à distance sur la branche de test ; `main`, firmware, archives, configuration et matériel sont inchangés.
