---
mission_id: TR2-20261019-UXL2-PHYSICAL-LIVE-UI
status: READY
base_ref: main
base_sha: 295fefec96f678b588c7790ba391097084feeefb
created_at_utc: 2026-10-19T09:45:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Acquisition physique continue et visualisation dans la supervision UX-L2

## Contexte et objectif

L'utilisateur a **explicitement demandé à observer les courbes se construire en temps réel** avec son TR2 connecté, et non de consulter de nouveau l'archive historique. Les écrans UX-L2 ont été compilés dans un worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-TEST-IHM`, et la relecture archive R5-B2 a été observée humainement dans Edge sur le port 5094. Rapport UX-L2 `DONE`, archivé sous `archives/TR2-20261018-UXL2-LIVE-TRENDS-NAVIGATION/`, code `e5537ebe2b1898da9d461339a425826754590154`.

**But de cette mission :** préparer et, si l'état physique le permet, exécuter une **campagne bornée de 5 à 10 minutes environ** de mesures réellement renouvelées depuis le capteur IIS3DWB/STM32, transport RS-485 COM4, service Windows PC, SQLite neuf, API `/api/v2/live` et `/api/v2/live/history`, navigateur Edge **sur les pages UX-L2 réelles**, pour que l'utilisateur puisse regarder la tendance évoluer pendant l'acquisition. La durée est indicative : privilégier sûreté et critères mesurables aux minutes exactes ; une courte répétition bornée est permise pour corriger un défaut constaté. **Ne pas qualifier la démonstration comme profil produit, endurance, métrologie ou R6.**

## Obligations de démarrage et base technique

1. Vérifier `main`, `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole échanges, document UX et rapport actuel, état technique et roadmap, `LIVE_R5B2.md`, code/préuves R5-B2, protections M016 et livrables UX-L2. Préserver le dépôt local historique divergent `82169af`, le worktree de test, tous autres worktrees, stashes et archives. Utiliser un worktree isolé si approprié et ne jamais traiter les données historiques archivées comme actuelles.
2. Le firmware **M016 a été restauré** le 18 octobre : Flash M016 SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`, FRAM finale SHA `2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84`, journal 78/root91, epoch5/run11, STOP 5/13, acquisition arrêtée, GDB débranché/COM fermé **au dernier contrôle historique**. État matériel **à revérifier** avant action. M016 n'expose pas directement le profil `LiveDiagnostic` : sélectionner une **image diagnostique exacte, contrôlée et récupérable** si nécessaire, sans retoucher la V1 normative ni supposer que le probe précédent est encore en Flash.
3. Évaluer le câblage/alimentation/ST-LINK/COM4, tension et cible par contrôles non destructifs permis. Toute **intervention physique humaine** indispensable doit être spécifiée très précisément à l'utilisateur ; ne pas supposer le matériel présent ou correctement branché. Aucun flash/debug sans identification de cible, image, empreintes et chemin de retour fiable.
4. **Garde matérielle :** préserver identité, FRAM/son historique, microSD, Option Bytes, protections et Flash de référence ; aucun mass erase, aucun effacement global ni écriture à l'aveugle. Conserver un oracle préalable proportionné, FRAM attendue post-R5B2, preuve de récupérabilité M016. Sauvegardes supplémentaires et restauration finales uniquement proportionnées aux risques ; une restauration sûre de M016 est attendue après la campagne si un probe temporaire remplace la Flash, avec état final relu. Aucune mise à jour destructive inhabituelle. Ne pas réutiliser les clés B5 **5/12 et 5/13 déjà consommées**, ni rejouer `hardware/run.py` ou les scripts historiques inchangés ; nouvelle garde transactionnelle, clés/epoch/run admissibles et modèle FRAM frais établis de façon vérifiable. Garder les contraintes de temps START/STOP auparavant ~40 secondes.
5. **Un seul maître RS-485** par phase, accès exclusif COM4, 115200 8E1 réellement mesuré R5-B2, fermeture du port entre les séquences de contrôle B5 et le service d'acquisition. Le navigateur et service n'émettent aucune commande cachée. Pas de polling simultané en conflit. Ne pas relancer la copie déployée `TR2-r5b` en parallèle du service UX-L2.

## Campagne et supervision LIVE

6. **Avant de lancer le matériel**, prouver la composition du **vrai service UX-L2 sous Windows** avec `--live-direct COM4 --allow-diagnostic`, base SQLite **nouvelle** hors répertoire de preuves protégées, listen localhost sur port libre, trace neuve, contrôles sur durée/arrêt, et endpoint historique lié à **cette même base vivante**, pas à `--live-archive`. Vérifier le chemin de compatibilité Windows/WSL et l'exécutable réellement construit, le profil LiveDiagnostic et la différence entre `/api/v2/live`, `overview` et `history`. Corriger de façon minimale une lacune réellement démontrée avant essai ; ne pas déclarer LIVE l'ancien service d'archive actuellement sur 5094.
7. Avec matériel identifié et garde FRAM/Flash démontrée, déclencher START dans le protocole B5 existant par maître explicite et distinct, exécuter la campagne pendant un intervalle borné puis STOP et vérifier le terminal. Le service Web et la fiche `/live-device.html?deviceId=<identité réellement observée>&view=vibration` doivent être observés **pendant que les observations arrivent**. Vérifier les transitions initial / courant / HELD et le statut `isCurrent` réellement calculé ; ne pas prétendre qu'une mesure archivée FRESH est courante sans réception directe. Réception PC horodatée, segments, échantillons et continuité exacts.
8. Tests de la chaîne réelle : mesures B3 **distinctes et renouvelées** (RMS/crête globales et X/Y/Z mg) sur SQLite; nombre et cadence observés; API `overview` et `history` dans le **même process**, vrais DOM Edge avec variation des points graphiques entre deux instants alors que acquisition active. Tracer l'état acquisition, communication, qualité V/F, HELD après STOP, absence d'UTC fenêtre, sévérité B3 et différencier réception PC vs temps fenêtre. CSV du lot réellement affiché et comparaison des lignes/valeurs avec SQLite/API si possible. **Au moins deux instantanés DOM horodatés pendant RUNNING** avec compte de points croissant, à défaut déclarer preuve d'actualisation insuffisante et qualifier seulement les étapes réellement démontrées.
9. Le projet UX-L2 est encore une IHM **read-only en profil `LiveDiagnostic` NON-PRODUCTION**. Ne jamais connecter ce scénario à un équipement réel du bord ni induire une politique de seuils industrielle. Aucune vibration imposée/expérimentation risquant d'endommager le capteur ; aucun besoin d'action physique utilisateur au-delà du branchement/mise sous tension si nécessaire. Ne pas altérer les données historiques, la spécification figée ou les commandes S7-B5.
10. Préparer pour l'utilisateur un **mode de relance opérateur simple** (service Windows UX-L2, source des captures, URL localhost et procédure d'arrêt), clairement borné et sans clés B5 historiques. Si une vraie commande START/STOP doit rester menée par Codex, expliquer précisément ce qui est automatisable et ce qui exige le matériel et la présence de Codex. Ne pas laisser une acquisition/microcontrôleur ou un port COM ouvert sans état documenté en fin de mission.

## Validation, conditions d'arrêt, livrables

- Contrôles hôte/Windows et tests JS/.NET **ciblés**, compléments proportionnés aux erreurs réelles. Éviter de recompiler/tester firmware inchangé ou rejouer toute la validation UX-L2 sans justification.
- Preuves horodatées brutes et corrélées : SHA Git/code/image exécutable ; identité carte ; Flash/FRAM avant-après si intervention ; commandes distinctes et résultats B5 terminaux ; métriques série/SQLite/HTTP ; 2+ DOM/graphes Edge pendant acquisition ; STOP/HELD final ; état COM/debug et restitution M016 si probe flashé.
- Conditions d'arrêt : impossibilité d'identifier matériel/câblage, COM occupé, absence du STM32 ou d'une image fiable, oracle FRAM qui diverge, doute de récupération, conflit de permissions, erreur de sécurité temporelle/stockage, corruption/incohérence. En cas de blocage réel : publier `BLOCKED`, checkpoint précis et **une seule demande physique ciblée** ; ne pas inventer l'essai. Travaux PC faisables autonomes d'abord, sans attendre inutilement.
- Documentation opérateur et observations factuelles, corrections minimales si nécessaires, commit/push normal sur `main`, `DERNIER_RAPPORT.md` avec statut réel et preuves publiées/relecture distante. Garder UX-L3 (overlays), R6/endurance et conformité industrielle hors champ. ChatGPT fera le contrôle indépendant avant qualification.

**Autorisations explicites :** `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true` selon garde-fous matériels de `AGENTS.md`. Autonomie de corrections et de flash/debug **proportionnés** accordée, sans validation répétitive. L'utilisateur conserve les gestes physiques et risques inhabituels non maîtrisés.
