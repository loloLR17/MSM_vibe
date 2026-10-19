---
mission_id: TR2-20261019-WINDOWS-TRUST-LIVE-RESUME
status: READY
base_ref: main
base_sha: 0d0c395dc0cf5f68193d3bade6df8fcdc75ad092
created_at_utc: 2026-10-19T11:15:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Reprise prioritaire — déploiement Windows autorisé, puis démonstration physique TR2 LIVE

## Situation et continuité

La mission antérieure `TR2-20261019-UXL2-PHYSICAL-LIVE-UI` révision 2 est **BLOCKED** (non DONE), avec rapport et contrat inchangés **archivés intégralement** sous `Modbus RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261019-UXL2-PHYSICAL-LIVE-UI/`. Le checkpoint technique dans `Supervision TR2/operator-demo/REPRISE.md` reste référence factuelle. L'archivage clôt son rôle de contrat actif **sans requalifier le statut BLOCKED**. Le présent contrat autorise une nouvelle tranche, distincte et ciblée sur le déploiement Windows puis sur les essais physiques restants. Vérifier le HEAD et toute évolution factuelle sur `main` avant exécution.

**Priorité utilisateur : autonomie maximale de Codex** pour le déploiement des DLL, sans lui transférer les opérations techniques réalisables ni affaiblir la sécurité du T1000. Objectif de démonstration dès que physiquement possible : STM32 avec **firmware LIVE laissé installé**, START/STOP répétables via les lanceurs, courbes renouvelées en Edge à partir de COM4/SQLite, durée sans minuterie 5–10 minutes. Ce n'est ni un GO industriel ni une preuve d'endurance illimitée. Le lot ergonomique et le nettoyage des nombreux worktrees sont à traiter **après** la chaîne LIVE fonctionnelle, sans opération destructive sur l'environnement au cours de cette mission.

## Faits et acquis à préserver

- Code opérateur `c36c9718a75e6b40c45707a482e8db836296cab2` : firmware autonome STOP au boot, sans sortie temporisée, lanceurs `LANCER_SERVICE.cmd`, `OUVRIR_EDGE.cmd`, `START.cmd`, `STOP.cmd`, `ARRETER_SERVICE.cmd`, service Windows `--seconds 0`, pause tokenisée/exclusivité COM, B5 transactionnel avec allocation durable avant submit et réponse terminale+effet.
- Windows : déploiement autonome `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\`, 382 fichiers vérifiés. Smoke Windows sur COM99 inexistant **échoue avant accès série** : FileLoadException `0x800711C7`, `TR2.Domain.dll` SHA-256 `43feefed9b8c7dfd2917051fa9363aea27b760e7ad2a897cd1005412762f0514`, Code Integrity événements 3033/3077, policy `{0283ac0f-fff1-49ae-ada1-8a933130cad6}`, DLL non signée et exigence signature Enterprise. Le `dotnet publish` réussi ne prouve pas son exécution Windows.
- Même machine : démonstration **R5-B2** a réellement exécuté un service Windows antérieur depuis `C:\Users\lolo\AppData\Local\MSM\TR2-r5b\`, sans modification de politique documentée. Source de Domain inchangée, DLL nouvellement produite différente. **Ne pas présumer la raison de la confiance de R5-B2 ni confondre nom de répertoire et autorisation.**
- Préflight STM32 frais du 19/10 10:04UTC : M016 encore flashé SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26` ; FRAM entière SHA `2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84`, journal78/root91/epoch5/run11, clés 5/12 et 5/13 consommées ; aucun nouveau flash, COM4, START/STOP ou acquisition dans le checkpoint. BIN candidat `tr2-live-operator-NON-PRODUCTION.bin` SHA `9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981`, compilé mais **non flashé**.
- 350 tests .NET hors Service puis 238 Service finaux, cross-build et construction ARM, publication win-x64, smoke Linux sans matériel acquis : **ne pas relancer sans raison**. Deux stashes, `main` local historique divergent `82169af`, worktree `MSM_vibe-TEST-IHM` et 70 autres worktrees préservés. Aucun nettoyage de masse pendant cette reprise.

## Phase 1 — identifier et résoudre durablement le déploiement Windows

1. Revoir gouvernance `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole, R5-B et R5-B2, checkpoint BLOQUÉ et fichiers de preuve `windows-code-integrity.json.zlib`, `windows-domain-provenance.json`, `windows-deployment.json.zlib`, ainsi que les binaires conservés. Enquêter **activement** sur le traitement qui a rendu le déploiement R5-B2 chargeable, sans le supposer. Comparer anciens et nouveaux fichiers : empreintes, signatures/Authenticode, éditeur, provenance, propriétés du paquet, règles WDAC/App Control, journaux correspondants, configuration de distribution et identité réellement autorisée. Vérifier de façon non destructive la politique effective du T1000 et ce qui est permis dans cette session.
2. Chercher et **mettre en œuvre de manière autonome**, dans les droits effectivement disponibles, la **voie d'autorisation officielle adaptée** : publication signée avec certificat/chaîne de confiance légitime déjà approuvés, procédure d'approbation par l'administrateur ou canal officiel de déploiement d'entreprise, règle d'autorisation ciblée selon gouvernance Windows existante, ou autre solution supportée par le mécanisme réel et explicitement permise. Automatiser les opérations reproductibles que Codex peut effectuer ; éviter de redemander une validation pour des actions déjà autorisées. Une simple copie dans un ancien chemin, un renommage, un déblocage ADS, une autre instance de runtime ou une signature auto-générée **ne constitue pas à elle seule une autorisation démontrée**.
3. **Interdits :** désactiver/contourner WDAC/App Control/Code Integrity, altérer une politique ou créer une exception locale hors droits d'approbation, utiliser un emplacement/processus de confiance pour éluder une politique, masquer le refus, désactiver protections, emprunter des signatures ou certificats sans autorisation, ou considérer une exécution Linux/replay comme preuve Windows. Pour toute autorisation externe véritablement requise, fournir une **seule demande précise** à l'utilisateur (acteur, commande/console officielle, éditeur/certificat/règle/fichiers exacts, privilège, preuve d'approbation). Ne pas imposer à l'utilisateur de construire/copier lui-même les DLL si Codex peut s'en charger.
4. Critère d'acceptation **avant flash** : le **nouveau binaire Windows** se lance réellement sur le T1000 avec **smoke COM99 inexistant** (aucun TR2 sollicité), sans `FileLoadException` et sans événement de blocage Code Integrity corrélé ; le résultat attendu n'est pas une connexion série COM99 mais un lancement effectif du runtime, du Web et des DLL. Capturer Windows Event Log, exe et dépendances, commande, horodatage, SHA des DLL, résultat HTTP et justification de confiance. Démontrer que la méthode reste utilisable **pour une recompilation future**, sans promettre un auto-allow universel.

## Phase 2 — reprendre le LIVE physique avec contrôles conservés

Une fois la Phase 1 **prouvée**, suivre `operator-demo/REPRISE.md` et contrat archivé sans refaire les travaux acquis. Vérifier état actuel cible/FRAM/Flash/identité/COM4 et images de récupération de façon proportionnée. `allow_flash: true`, `allow_debug: true` permettent build, flash, debug et corrections minimales nécessaires conformément aux garde-fous : préserver FRAM/microSD/identité/Option Bytes, aucune opération destructrice inhabituelle, aucune ancienne clé B5, récupérabilité M016 démontrée. Un seul maître exclusif COM4, pas de GDB bloquant pendant FIFO.

Flasher le firmware LIVE opérateur connu ou une correction explicitement construite, vérifier empreinte Flash/boot STOP, et **le laisser sur la carte** s'il est sûr et fonctionne. Le service UX-L2 Windows utilise sa **propre nouvelle** SQLite vivante et port localhost dédié (5100 prévu), pas l'archive WSL5094. Valider série → B3 réel → SQLite → API history/overview → Edge et CSV. Premier cycle START/STOP à réponse **terminale+effet** sans faux succès HTTP; STOP ~30–60s accepté, mais vérifier état final; puis second run **prolongé sans arrêt automatique arbitraire**, jusqu'à STOP volontaire et surveillé, avec durée réellement démontrée, erreurs FIFO/transport, croissance archive et identité/admission FRAM suivies. Deux instantanés HTTP/DOM avec nombre de points croissant et mesures réellement renouvelées ; ne pas interpoler les lacunes ni faire passer HELD/historique pour courant. Ne laisser ni run sans surveillance, ni COM/GDB en mauvais état. Documentation de démo avec lanceurs START/STOP, statut réel, chemin des logs et procédure opérateur simple.

## Arrêts, publication et responsabilités

Si phase 1 ne peut être achevée sans approbation extérieure, **ne pas flasher à l'aveugle**, poursuivre les vérifications logicielles utiles, publier `BLOCKED` et checkpoint reprenable **sous ce même identifiant**, sans inventer de test physique. En cas de conflit d'intégrité/matériel, anomalie FRAM, impossibilité STOP terminal ou problème sécuritaire, stopper de façon maîtrisée et escalader seulement le point non résoluble. Aucune suppression d'ancien worktree, stash, archive ou main local.

Résultats attendus : cause Code Integrity argumentée, méthode de déploiement officielle et degré réel d'autonomie de Codex, scripts/procédure réutilisables, smoke Windows prouvé, essais physiques si débloqués, SHA Git et artefacts, métriques, erreurs/limitations, état matériel final, rapports et preuves lisibles. Commits/push normaux sur `main`, documentation opérateur mise à jour, rapport `DERNIER_RAPPORT.md` publié et relu à distance. ChatGPT assure contrôle indépendant et décide la qualification, distincte de `DONE`.

**Autorisations explicites :** `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Autonomie étendue aux tâches et déploiements conformes aux politiques réellement applicables ; l'utilisateur garde les gestes physiques et approbations système importantes hors des droits présents. Priorité démonstration LIVE ; nettoyage Git/ergonomie restent missions séparées.
