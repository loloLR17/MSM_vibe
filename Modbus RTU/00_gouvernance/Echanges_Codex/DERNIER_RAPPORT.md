---
mission_id: TR2-20261028-ACCEL-FS8G-VIBRATION-AC
status: BLOCKED
base_ref: main
base_sha: e7f17f2cd2b8470244a99f7c9e73e9d0f43b3dea
initial_head: afa7d707b50d56b771c3dcb7bf719ab66aa9cbb6
result_sha: ae6e2d5e9cd2d161abeda1659d99296a0c274ab8
created_at_utc: 2026-10-29T06:20:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# AC1 / ±8 g — logiciel publié, qualification physique bloquée par Windows

La mission reste **BLOCKED**, sans clôture technique : le nouveau paquet Windows complet R2 échoue au contrôle de confiance. Le correctif de lecture AC1 est publié et testé ; **ni ±8 g effectifs ni le retrait du DC en fonctionnement LIVE ne sont physiquement qualifiés**. L’addendum impératif36fd068 et l’autorisation utilisateur AC1 sont pris en compte : ces deux objectifs sont prioritaires et ne sont pas remplacés par une étude hors ligne.

L’ancien service accepté fonctionne de nouveau en observation B3 sur COM4, seul propriétaire. Le STM32 répond STOP, sans mesure valide après démarrage ; aucun nouveau START/APPLY n’a été envoyé. Le nouveau firmware reste installé, arrêté, sans restauration automatique de M016. Les historiques et les paquets Windows précédents sont préservés.

## 1. Reprise et état Git

- Reprise après interruption de session, sans refaire l’investigation ni les validations réussies. Worktree `MSM_vibe-accel-fs8g-vibration-ac`, branche `codex/accel-fs8g-vibration-ac`, HEADf953efbc : les deux modifications non committées y restent intactes, également sauvegardées dans `resume-transport.patch` sous les preuves locales.
- `origin/main` vérifié à bdaa33353264e81c973c185555f2e5d8e94ab364 : descendant du checkpoint, changements limités à trois documents R0 métier. Worktree neuf `MSM_vibe-accel-fs8g-ac-resume`, sans merge/rebase/reset ni perte de travail ; réapplication du seul correctif vérifié.
- Code72a7b57381f61e9f24c6bdcd20dd70a38dbc70de et checkpointf953efbc déjà publiés. Correctif de frontière PC **ae6e2d5e9cd2d161abeda1659d99296a0c274ab8**, puis preuves de reprise **d5a89e22**, publiés par push normal et références distantes contrôlées.
- Main a ensuite reçu9011c6a4/ef3b33a33830bd28a85b2f92385cf8d71749e023, deux documents métier sans changement du contrat ou du code testé. Le worktree propre passe sur une nouvelle branche `codex/accel-fs8g-ac-report` basée sur ce descendant ; l’ancienne branche est préservée. Aucun historique divergent n’est fusionné.
- Ancien dépôt principal maintenu propre à82169af ; 76 worktrees après l’ajout, aucun nettoyage. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 et f3b829e6188f491920edf74a5f9ed0f559d08b87 conservés.

## 2. Modifications et validations logicielles acquises

Le code firmware72a7b573 ajoute un traitement XYZ continu distinct : passe-haut complémentaire du premier ordre, coupure nominale2Hz à26667Hz, estimateurDC en Q16µg initialisé au premier sample après START, préparation26667samples, fenêtres N/P. RMS/crêtes XYZ et global sont calculés sur le signal dynamique ; la crête globale correspond à un vecteur effectivement échantillonné. Filtrage maintenu entre fenêtres, validité réinitialisée au START, HELD après STOP, invalidation en cas de faute/rail/perte d’autorité. Sensibilités ±2/4/8/16g :61/122/244/488µg/LSB. Le retraitDC n’est pas un étalonnage et atténue les accélérations lentes ; une rotation rapide peut laisser une composante transitoire.

L’extension **AC1v1, FC03 8000–8063**, autorisée explicitement par l’utilisateur, est opt-in et exclusivement opérateur NON-PRODUCTION. Absence de collision vérifiée ; modèle canonique, LIVE R0 gelé et sens de B3 absolu inchangés. Métadonnées méthode/FS/DC/run/config/séquence/qualité, RMS/crêtes dynamiques séparées dans le protocole expérimental, SQLite JSON, API, UI et CSV. Pas de retrait arbitraire1000mg du RMS ni de reconstruction depuis B3.

Le chemin préparé `apply8g` copie l’active B4, change seulement FS=2, crée un nouvelID/CRC, relit le staging puis soumet le vrai APPLY transactionnel. Ledger persistant avant écriture et handoff tokenisé/exclusif conservés ; aucune écriture FRAM brute. La calibration temporelle de boot reprend le FS configuré, sans imposer ±2g ; elle n’assure toujours aucun étalonnage physique.

**Défaut trouvé sur le vrai transport PC :** R1 lisait B0/B3, puis refusait AC1 localement avant émission, avec «Outside bounded readonly LIVE registers.». Le lecteur applicatif possédait son option mais la liste du transport série n’avait pas été étendue. ae6e2d5e transmet l’opt-in au transport et autorise uniquement le tuple8000/64 sur `live-direct`, sans writer ni intervalle arbitraire. Les refus sur8000/65,8001/63, autre bus et mode sans opt-in sont testés.

Résultats conservés et vérifiables, **non relancés à la reprise sur code inchangé** :

- 148 tests C et cross-build canonique PASS via `STM32CUBE_U5_ROOT=… bash ./tr2_validate.sh` ; invocation initiale directe refusée126 car script non exécutable, sans chmod ni résultat masqué.
- Image opérateur ARM compilée : text150484/data492/bss659176. ASan/UBSan O2,3 suites PASS ;448 oracles B3 inchangés. Rejeux déterministes DC,20/50/1000Hz à500mg et100Hz à6500mg, quatre FS/rails/STOP/restart/faute. Ce sont des preuves logicielles, pas une excitation physique ±6g.
- .NET :305 autres tests PASS, puis Protocol52 et Service252 PASS ; après correctif de frontière, **Service253/253 PASS**, rouge/vert conservé. Suites séparées, aucune nouvelle exécution globale verte prétendue.
- Tests JS AC1 et non-régressions tendances/métrologie/overview PASS. Nouvelle compilation Windows complète R2 réussie ; aucune compilation C supplémentaire nécessaire pour le seul correctif PC.

[Conception AC1 et décision](../../Supervision%20TR2/operator-demo/AC1_V1.md), [preuves logicielles](preuves/TR2-20261028-ACCEL-FS8G-AC1/software/README.md), [rouge/vert et preuves de démarrage](preuves/TR2-20261028-ACCEL-FS8G-AC1/resume/BUNDLE.json).

## 3. Opérations matérielles effectivement réalisées avant l’interruption

Cible TR2-001, ST-LINK003500463235510B37333439, UID004A003D/38425018/20333157. COM4 FTDI est le port RS485 ; COM3 ST-LINK n’est pas utilisé comme Modbus.

1. B3 STOP et dernier terminal5/25 vérifiés avant intervention. Sauvegarde sûre hors scope chronométré : deux lectures Flash complètes2097152octets égales, SHA256 **6832166a31ae8c5ed1e41cea3da141e6db9120f91ea0e6b6d9f40ba8daada51f** ; FRAM entière SHA256 **3c56ea55bbe01cc31fdd71e13bcc5a5af510ab2c8205f893d6c2dad22473a2b5**, historique/queue conservés. Puis vraie réponse B0/B3 après detach, sans reset, à22:40:49UTC le28/10.
2. **Un flash standard** qualifié de l’image72a7b573, BIN150976octets SHA256 **e1846c87017fd7667daf1b79cb440af1024346c5deae8d5065f8525c00aa7b80**, à0x08000000, vérification et reset de programmation. Aucun mass erase/OptionBytes/SD. ELF SHA2566dddfaafb604e0a19edc0f4c3d4c66bafd4049c6e5476069982934c5ca35b012.
3. **Erreur opérateur conservée :** un contrôle GDB de lecture trop tôt après flash a halté le CPU pendant la calibration FIFO. Le premier démarrage est invalide, pas qualifié : diagnostic ultérieur r4u.error3/r4r.error8/diagFIFOerror7, FIFO arrêté, aucune acquisition active, journal126/STOP5-25/config3gen3 inchangés dans cette lecture. La causalité avec l’interruption GDB est cohérente avec les observations, pas une preuve de panne spontanée du capteur.
4. **Un reset logiciel de récupération**, justifié par cet état terminal arrêté et gardé par UID/noactive/FIFOoff, sans reflash ni patchFRAM. Après detach et fin du boot sans GDB, B0/B3 répondent ; le nouveau refus AC1 est local au PC, distinct du premier échec de démarrage.

Ces audits sont datés. La FRAM complète n’est pas déclarée relue après le reset sur la seule absence de commande. Aucun nouveau debug/flash/reset n’a eu lieu pendant la reprise du29/10 ; aucune nouvelle commande B5 dans toute cette mission à ce checkpoint.

## 4. Blocage Windows effectivement observé

R1 complet `TR2-AC1-FS8G-20261028\app`, code72a7b573 : gate28/10 PASS,378fichiers égaux,4HTTP200/SAC1/zéro refus corrélé. Cela ne garantissait pas une future publication.

R2 complet séparé `C:\Users\lolo\AppData\Local\MSM\TR2-AC1-FS8G-20261028-R2\app`, codeae6e2d5e : publication et copie des378fichiers vérifiées, aucun mélange DLL. **Gate29/10 06:16:08–06:16:40UTC FAILED** sur COM99 absent, sans accès au TR2 :

- HTTP503 lié au stockage SQLite indisponible ;4HTTP200 non obtenus, malgré code retour0 du processus. Le retour0 seul n’est donc pas une validation.
- **296 événements3033/3077 corrélés** :292 visant `NModbus.Serial.dll`,4 visant `SQLitePCLRaw.batteries_v2.dll` ; deux composants distincts, pas296DLL.
- Politique0283ac0f-fff1-49ae-ada1-8a933130cad6, SAC1 ; `NModbus.Serial.dll` SHA2561714c12265bf336cff5fd776f97f922fea9b3f0291ccd06e5ccc6c23d41be1c0. Les deux DLL tierces sont byte-identiques à R1 ; aucune cause unique de réputation/signature n’est déduite de cette égalité.
- Aucune protection modifiée, aucun déblocage ADS/EA, exception, mélange de fichiers ou tentative de contournement. R2 n’a pas été lancé sur COM4.

Ce refus interdit de poursuivre la qualification avec R2. Décision attendue : fournir une voie de confiance/signature/déploiement approuvée pour le paquet complet, sans désactivation ni contournement de SAC. Ne pas répéter le gate tant qu’aucun changement pertinent n’est apporté. L’ancien paquet accepté est utilisable pour observer B3, **pas pour prétendre qualifier le canal AC1 absent de ce service**.

[Manifestes, gate, événements structurés, état final](preuves/TR2-20261028-ACCEL-FS8G-AC1/windows-r2-block/README.md). XML intégraux conservés localement ; extrait public sans SID ni nom du PC, avec ID/record/UTC/composant/statut/politique/hash.

## 5. État final et conservation des données

- R1 a été arrêté le29/10 à06:17:36UTC après preuve B3 STOP fraîche, ledger5/25, verrou opérateur exclusif et acquittement tokenisé de fermeture réelle COM4. Arrêt limité à son propre processus readonly ; `.shutdown` seul ne pouvait satisfaire le critère de transport sain à cause du refus AC1 local. Aucun FC10 pour effectuer cette transition.
- Sauvegarde SQLite R1 par API native Windows, intégrité `ok`, **75006observations**. Comparaison SQL intégrale : les **52824lignes originales sont présentes et identiques**, zéro ligne perdue/modifiée. SHA256 du snapshotR1 **7842166e89c99db49d7ea81f3db37d26b577932fefbfdeb63f30be51f28ab4df**. Les nouvelles observations R1, notamment Rejected/CommunicationLost, restent conservées dans sa base et sa sauvegarde ; elles ne sont pas fusionnées silencieusement dans l’ancienne base.
- Les378fichiers de l’ancien paquet et tous ses fichiers de données gelés étaient exacts avant relance. Ancien service `TR2-LIVE-COM4-HELD-20261019` relancé ; preuve réelle API à **06:18:59UTC** : transport connecté, TR2 STOP/flags4097/seq0, aucune mesure valide. Processus unique8312 à06:19:12UTC, SAC1/zéro refus corrélé à cette relance. Les lanceurs habituels pointent toujours sur ce paquet, inchangés. Son archivage reprend dans sa base initiale préservée.
- Firmware e184 installé conservé ; pas de restauration M016. Dernier FS persistant matériel lu avant nouvelle image :config3/gen3/±2g ; aucune commande APPLY n’a été envoyée. **Ne pas annoncer ±8g actif.** L’état STOP final est observé par B3 ; aucun audit FRAM long supplémentaire n’est présenté comme effectué.
- FRAM/identité/journal/epoch, microSD, OptionBytes, anciens paquets/bases/archives/worktrees/stashes préservés. La confirmation finale de FRAM intégrale et les marges temporelles AC1 sous acquisition restent à faire, sans transformer l’absence d’écriture volontaire en une relecture physique.

## 6. Point de reprise, sans répétition des acquis

1. Lever le refus Windows par un mécanisme approuvé, puis gate complet4HTTP200/SAC1/zéroCI sur le paquet effectivement utilisé. Réutiliser le codeae6e2d5e, firmwaree184 et tests conservés ; ne pas recompiler/reflasher sans motif.
2. Relever état cible/ledger/propriétaire frais. Handoff exclusif vers R2 accepté ; sauvegarde SQLite native et ledger courant, jamais recopier un ancien ledger après une commande. Conserver aussi les observations R1 intermédiaires.
3. Qualifier une baseline courte du nouveau traitement à±2g, START/STOP terminaux et AC1 réel. Puis vraiAPPLY8g, activeB4/CRC/ID relus, reset logiciel prévu pour persistance **seulement à STOP/noactive**. Après reset, attendre le boot via le bus : aucun halt GDB pendant calibration.
4. START±8g, preuve physique CTRL1_XL=0xAC par cache matériel contrôlé, AC1 valide/frais et B3 absolu séparé. Capteur immobile : quantifier DC, bruit résiduel dynamique, comparaison2g/8g. Vérifier conversion et marge avec les rejeux déjà acquis ; **aucune excitation machine±6g n’a été réalisée**.
5. Prouver COM4→SQLite/API→Edge/CSV en RUNNING puis STOP/HELD, conservation intégrale des historiques, budgets temporels/FIFO/UART. Audit final horsscope/FIFOoff si nécessaire, puis vraie réponse bus après detach sans reset. Conserver firmware cumulatif si sain. START/STOPWeb reste P2 distinct.

Journal `ETAT_COURANT_TR2.md` et document opérateur `REPRISE.md` actualisés pour remplacer les états périmés et rendre le blocage explicite. Aucun changement normatif ni qualification indépendante revendiquée. Le présent rapport sera relu intégralement depuis GitHub après push ; SHA et état Git fournis ensuite. Preuves locales : `/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac/`.


## Publication distante vérifiée

Push normal du bilan **3c59d32fdb7c6be30ba8397cb9b3a68a38004332** sur main réussi. Référence récupérée de GitHub le29/10/2026 à06:22:45UTC : **34 fichiers relus intégralement et byte-identiques**, dont rapport/code/tests/journal/reprise ; **26 artefacts** des bundles de reprise et de blocage vérifiés après décompression avec empreintes brutes et stockées. [Trace de relecture](preuves/TR2-20261028-ACCEL-FS8G-AC1/windows-r2-block/remote-readback-resume.json). Cette publication ne lève pas le blocage Windows et ne qualifie pas AC1/±8g.

État Git au bilan : worktree de publication propre, HEAD égal à main distant ; dépôt historique propre82169af et ancien worktree avec les deux changements initiaux préservés. Le SHA de la mise à jour documentaire consignant cette relecture sera fourni dans la réponse finale après contrôle distant. Statut BLOCKED maintenu, mission reprenable sous le même identifiant.
