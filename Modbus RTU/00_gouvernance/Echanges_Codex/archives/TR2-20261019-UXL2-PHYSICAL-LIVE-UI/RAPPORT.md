---
mission_id: TR2-20261019-UXL2-PHYSICAL-LIVE-UI
revision: 2
status: BLOCKED
base_ref: main
base_sha: 0e992dc49c7fe803b40d7f4a45a6968c094258d2
initial_head: 793748645e257a57ccdad48afca855ba54072594
result_sha: c36c9718a75e6b40c45707a482e8db836296cab2
created_at_utc: 2026-10-19T10:17:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Révision2 — LIVE opérateur préparé, qualification physique bloquée par Windows

**BLOCKED : le contrôle d'application du T1000 refuse le nouveau service Windows avant toute ouverture série.** L'utilisateur a explicitement demandé « Publie le blocage et les acquis ». Ce statut n'est ni un échec STM32 ni une attente artificielle de validation intermédiaire. Les tranches logicielles réalisables sont préparées, testées et publiées ; **la démo physique du20/10 n'est pas validée**.

**La carte n'a pas été flashée ou réinitialisée.** M016 était encore présent au préflight et n'a jamais été remplacé ; aucune restauration M016 n'a été effectuée. Le firmware LIVE autonome préparé ne peut pas être annoncé utilisable sur la carte avant qualification du contrôle START/STOP et du service Windows. Installer l'image alors que son contrôle opérateur Windows ne peut pas démarrer aurait laissé l'objectif principal non démontré ; ce risque a été évité sans modifier la politique de sécurité.

Code préparé/évalué **`c36c9718a75e6b40c45707a482e8db836296cab2`**, `feat(live-demo): add persistent stopped-boot firmware and exclusive START STOP operator`, publié par push normal explicite sur main. Mission révisée7937486 remplaçant la révision1 à0e992dc. Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-UXL2-physical-live-ui`, branche auxiliaire `codex/uxl2-physical-live-ui`. Main historique82169af, TEST-IHM, autres worktrees et deux stashes préservés. Gouvernance, contrat et archives non modifiés.

## 1. Blocage actuel et action de reprise

Publication autonome win-x64 copiée localement et vérifiée : **382 fichiers** (application et lanceurs) dans `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\`. Le smoke sur **COM99 inexistant**, sans TR2, échoue avec retour82 avant accès série :

- `System.IO.FileLoadException`, HRESULT **0x800711C7**, message indiquant qu'une stratégie de contrôle d'application bloque le chargement.
- DLL refusée : `app\TR2.Domain.dll`, SHA-256 **`43feefed9b8c7dfd2917051fa9363aea27b760e7ad2a897cd1005412762f0514`**.
- Journaux Windows réellement relus : événements **3033 et3077**, politique **`{0283ac0f-fff1-49ae-ada1-8a933130cad6}`**, exigences de signature Enterprise ; événement3089 corrélable. Authenticode constate un fichier non signé. Le texte générique Authenticode relatif aux scripts ne remplace pas le diagnostic Code Integrity réellement observé.
- La source Domain est inchangée depuis R5-B2, mais le nouveau binaire diffère : cela n'autorise ni ne démontre sa confiance Windows. **Aucune politique, ADS, protection ou règle de sécurité modifiée ; aucun reconditionnement/chargement alternatif pour éviter le contrôle.**

**Action extérieure nécessaire : faire autoriser cette publication par le mécanisme de confiance prévu sur le T1000.** Aucune manipulation de carte ne lève ce blocage. L'utilisateur a choisi de publier le checkpoint plutôt que d'intervenir pendant cette session. Après résolution, constater un smoke Windows réellement réussi avant les opérations cible ; une déclaration d'autorisation seule ne vaut pas preuve d'exécution.

[Point de reprise complet](../../Supervision%20TR2/operator-demo/REPRISE.md), [mode opératoire et limites](../../Supervision%20TR2/operator-demo/README.md), [preuves](preuves/TR2-20261019-UXL2-PHYSICAL/README.md). La mission reste reprenable sous le même identifiant/révision, sans nouvelle mission ni répétition des validations acquises sur code inchangé.

## 2. Tranches réalisées — préparées, pas qualifiées physiquement

**Firmware opérateur distinct.** Nouveau flag explicite `TR2_STM32_LIVE_OPERATOR=ON` sur la recette matérielle R5B qualifiée, artefact `tr2-live-operator-NON-PRODUCTION`. Dans ce mode uniquement : boot autonome arrêté avec montage/validation réels R2 et calibration capteur, pas d'attente de trigger GDB, boucle de service sans sortie600s ou sortie après un STOP particulier. Les anciens probes gardent leurs SHA/identités fixes et bornes historiques. Le mode opérateur monte les autorités courantes avec contrôles CRC/identité/journal/effets, refuse recovery actif/indéterminé et ne rejoue pas d'effet. Budgets1000/2500/5000µs, FIFO unique, admission réelle et bornes d'écriture LIVE inchangés. La collecte finie des ranges du banc n'est pas utilisée comme autorité d'admission pour des cycles opérateur persistants ; les limites du journal R2 et les compteurs ne sont pas désactivés. Aucun RENEW automatique ou modification normative.

| Artefact préparé, non flashé | Taille | SHA-256 |
|---|---:|---|
| BIN opérateur |147024octets|`9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981`|
| ELF opérateur |1327744octets|`ba999ab4a8aa674a977ca8e3ab95469eba46c0d12ff2cdbf98c89bcfb1a1cbe3`|

Images et recette conservées dans les preuves ; provenance du build source c36c971 contrôlée. Compilation seulement : ne pas qualifier boot/STOP/tenue réelle depuis cet ELF.

**Service sans durée imposée.** `--seconds 0` signifie jusqu'à arrêt explicite ;1..600 restent les essais bornés précédents. Pas d'arrêt automatique5–10min dans le mode0. SQLite/overview/history sont ceux du même service vivant, pas une archive WSL substituée. Le poller reste readonly ; la nouvelle pause tokenisée acquitte uniquement après fermeture effective du handle COM. Lecture du contrôle compatible suppression/remplacement évitant les races Windows. Demande d'arrêt service exécutée seulement si le prochain B3 connecté déclare acquisition arrêtée. Fermer le service ne constitue toujours pas une commande STOP cible.

**START/STOP opérateur réutilisable, alternatif au writer Web.** Nouveau mode `--live-command start|stop --port COM4 --control <path> --state <ledger>`, réutilisant transport NModbus, requêtes canoniques B5 et moteur R2 cible. Un seul opérateur via lock, pause avec token et acquittement réel de libération, ouverture série exclusive, garde fraîche B0/B3/B5, allocation monotone `(epoch,txid)` persistée sur disque **avant submit**, aucun réemploi5/12–5/13, pas de retry d'effet/RENEW. Terminal exige epoch/tx/code exacts, statut4/résultat0 et absence d'actif ; effet B3 V/F pour START et HELD/nonFRESH/arrêt pour STOP. Affiche demande/en cours sans annoncer arrêt à l'acceptation. Fermeture writer avant reprise poller, y compris en erreur ; incertitude explicitement rapportée.

Le writer Web S7 cible un profil complet distinct ; le raccorder implicitement à ce profil partiel aurait changé une autorité non qualifiée pour cette démo. Les lanceurs sont l'alternative proportionnée autorisée, sans faux bouton Web, commande clandestine navigateur ou second maître permanent. Leur fonctionnement sur le banc reste à éprouver après levée Windows.

**Lanceurs portables préparés.** `LANCER_SERVICE.cmd`, `OUVRIR_EDGE.cmd`, `START.cmd`, `STOP.cmd`, `ARRETER_SERVICE.cmd` ; application autonome dans `app\`, données/journaux dans `data\`. URL prévue `http://127.0.0.1:5100/live-device.html?deviceId=1&view=vibration`, parc `/live-fleet.html`, distincte du serveur historique WSL5094. Pas de Git/recompilation demandé à l'utilisateur. README et reprise indiquent ordre opérateur, attente terminale30–60s, ledger à préserver, journaux et fermeture contrôlée. **Lanceurs non validés physiquement**, démo non prête tant que Windows bloque. Le répertoire `data\live.db` physique n'a pas été créé ; les smokes sont séparés.

## 3. Préflight réel et état matériel final

Préflight frais achevé **19/10/2026 à10:04:25UTC**, via chaîne GDB qualifiée, trois sessions ensuite détachées. Inventaire ST-LINK et FTDI Windows, aucun propriétaire TR2/GDB concurrent constaté ; COM3 ST-LINK distinct de COM4 FTDI0403:6001/BG04KSSW. Aucune ouverture COM4 ni transmission Modbus dans cette mission.

- NUCLEO-U575ZI-Q, **SN003500463235510B37333439**, UID **004A003D/38425018/20333157** vérifiés.
- Flash lue entière2097152octets : **`c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`**, exacte au M016 restauré R5-B2.
- FRAM lue entière262144octets : **`2b501a835b9a02c43dc0738bd7af167b7d049fbffb4e9ef677ed2e867f47dd84`**, exacte à R5-B2. Historique transactionnel non modifié, pas de rollback. Référence persistante correspondante journal78/root91/epoch5/run11/config3gen3/STOP5/13 ; les clés5/12 et5/13 restent consommées. Aucune nouvelle identité cible allouée/consommée.
- WHO_AM_I7B ; ODRoff, FIFO bypass et timestampoff réellement relus. CommissioningReadOnly/WAIT ; capteur arrêté. Sauvegardes fraîches Flash/FRAM vérifiées et voie récupérable M016 connues, jamais réécrites.
- **Aucun flash, reset, START/STOP, write FRAM, microSD, Option Bytes ou mass erase.** GDB détaché, COM4 non ouvert, aucun processus TR2/GDB restant à l'inventaire final. État final conservé depuis le préflight, pas de nouvelle lecture redondante après le blocage Windows.

M016 n'a pas été « restauré par défaut » : il n'a pas été remplacé, car le service et le moyen opérateur Windows ne pouvaient pas être qualifiés avant flash. **Temps RUNNING physique démontré :0s ; nouveaux points physiques :0 ; nouvelles commandes Modbus :0.** Aucun DOM Edge LIVE actuel, aucune preuve d'endurance ou de variation physique ajoutée. Les archives UX-L2/R5-B2 restent des acquis historiques distincts.

## 4. Validations effectives et erreurs conservées

- `STM32CUBE_U5_ROOT=... bash ./tr2_validate.sh --cross-build-only` : **CROSS-BUILD VALIDATED** canonique. Premier appel direct `./tr2_validate.sh` refusé126 car le fichier n'était pas exécutable ; exécution via bash sans modifier son mode, erreur conservée. Niveau proportionné : modification strictement dans harness/CMake, cœur portable inchangé.
- Recette CMake spéciale et build `firmware/` : **réussis**, text146532/data492/bss657400octets. Warnings newlib `_close/_fstat/_isatty/_lseek/_read/_write` visibles dans le log, pas des erreurs cachées. Aucun effet matériel déduit du build.
- Suite .NET complète initiale : **586 PASS** (350 hors Service+236 Service),0 ignoré ; après ajout des deux tests handoff et corrections du seul service, validation ciblée finale **238 Service PASS**,0 ignoré. **10 nouveaux cas** au total, allocation/epoch/épuisement/identité/terminalité et handoff simulé : token avant open, ledger avant write, trames exactes, writer fermé avant reprise, pas de retry après erreur. Ne pas présenter350+238 comme une suite complète588 rejouée après chaque changement. Premier build des nouveaux tests échoue CS0819 (déclaration `var` multiple), correction de déclaration, assertions maintenues et succès final conservés.
- `dotnet publish ... -r win-x64 --self-contained true -m:1 -o .../windows` : **réussi** ; copie locale et empreintes382 fichiers vérifiées. Aucun runtime installé globalement ou politique modifiée. **L'exécution Windows reste BLOQUÉE** ; une publication réussie n'est pas une validation de la démo.
- Smoke Linux réel sans matériel, COM99 inexistant, `--seconds 0` : processus vivant jusqu'à annulation, API locale, demande de pause/token/acquittement, reprise retirant l'acquittement, Ctrl+C et code0 propres. **Simulation de transport absent, pas Windows ni STM32** ; aucun test d'endurance revendiqué.
- Aucun test firmware/ancienne campagne physique ou DOM d'archive acquis rejoué pour fabriquer une preuve nouvelle. Les tests et builds supplémentaires portent seulement sur les changements de cette mission.

Logs/TRX, manifestes, scripts et images dans le nouveau dossier de preuves. Les échecs direct-script, CS0819 et Windows82 sont conservés séparément, pas remplacés par un succès. Pas de tests supprimés/désactivés ou résultats affaiblis.

## 5. Critères non satisfaits et reprise concrète

Restent **NON VÉRIFIÉS** : boot autonome de l'image LIVE sur carte ; START/STOP matériel par les nouveaux lanceurs ; redémarrage d'un autre run ; acquisition prolongée sans borne artificielle ; stabilité FIFO/Modbus/stockage ; service Windows actuel COM4/SQLite vivant ; deux DOM/HTTP RUNNING avec points croissants et variations mg corrélées à SQLite ; HELD après STOP ; firmware LIVE laissé en Flash et état final audité. Le blocage Windows explique ces absences, pas une panne cible démontrée.

Après autorisation Windows effective : smoke non matériel, garde cible actuelle proportionnée à la reprise, flash normal image identifiée/readback, premier cycle START/STOP, vérification boot autonome puis second run prolongé surveillé si sain, captures Edge/HTTP/SQLite, STOP final et contrôles FRAM/identité selon risque. Laisser LIVE flashé si sûr ; restaurer M016 seulement sur nécessité technique démontrée. **Ne jamais laisser une acquisition de test sans surveillance/STOP disponible.** La durée réellement obtenue sera mesurée ; aucune qualification de longue endurance ou capacité « illimitée » déduite d'une boucle sans minuterie. Ne pas relancer les anciens scripts R5-B2 à clés5/12–5/13.

Lot2 seulement proposé après lot1 : KPIs compacts, légendes/échelles et lisibilité1080p, statut demande vs effet, limites techniques repliables ; intégration Web transactionnelle à cadrer sous l'autorité existante. Aucun lot2 commencé pour masquer le blocage lot1. Pas de nouveau seuil/FFT/mm/s/calibration ou jalon industriel revendiqué.

## 6. Conservation et publication

**7929 fichiers protégés,70 autres worktrees dont TEST-IHM, deux stashes et main historique82169af préservés**, vérification complète par empreintes/HEAD/branche. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87 inchangés. Aucun reset/rebase/clean, force push, merge ou écrasement de travail utilisateur. Sources modifiées limitées au mode opérateur/PC/tests/lanceurs ; normes, gouvernance et archives intactes.

Journal `ETAT_COURANT_TR2.md` actualisé : préparation logicielle, blocage Windows, préflight/absence de flash, critères physiques manquants et suite. Constat factuel roadmap ajouté sans qualification de jalon. `allow_commit/allow_push` exercés et debug GDB de préflight exercé ; `allow_flash:true` **non exercé** faute de chaîne opérateur Windows qualifiable. Aucun besoin d'autorisation logicielle intermédiaire ; l'escalade porte sur le vrai contrôle de sécurité Windows externe au dépôt.

**Rapport BLOCKED publié puis relu intégralement depuis GitHub** : bilan `f5e640b6e31343adb77243c8f9d36b430a117eab`, récupéré à `2026-10-19T10:25:47.035474+00:00`, HEAD = origin/main = ls-remote ; **77 fichiers complets comparés**,59 empreintes de manifeste et57 artefacts BUNDLE vérifiés, décompression et contrôles des dumps/images/TRX/événements Windows compris. [Preuve de relecture](preuves/TR2-20261019-UXL2-PHYSICAL/publication-readback.json). Le statut reste BLOCKED malgré publication réussie : la preuve distante ne lève pas le contrôle Windows et ne qualifie pas le matériel.

Premier push du bilan interrompu côté RPC (`unable to rewind rpc post data`, curl65/connexion réinitialisée), distante encorec36c971 ; retry normal avec `git -c http.postBuffer=8388608 push origin HEAD:refs/heads/main` réussi. Réglage transport **par commande uniquement**, aucun changement permanent de configuration/sécurité. Les logs du retry sont publiés avec la preuve de relecture. La vérification tentée avant confirmation du premier push n'a pas été déclarée réussie ; seule la relecture ci-dessus est probante. La mise à jour de ce constat sera poussée et relue à son tour, son SHA fourni dans la réponse finale. Ce checkpoint n'archive ni ne clôt la mission comme DONE ; les autorisations restent limitées à son contrat et la reprise suit le document dédié. Contrôle indépendant ChatGPT distinct, aucun tag créé.
