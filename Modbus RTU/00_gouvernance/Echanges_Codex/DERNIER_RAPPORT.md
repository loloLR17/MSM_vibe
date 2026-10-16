---
mission_id: TR2-20261016-R4-COOPERATIVE-FRAM-FIFO-RT
status: IN_PROGRESS
base_ref: main
base_sha: 66c5e167b31cbc7baa302843c354760569d1c88d
initial_head: 178f3481e087110ef3f897f02bc8077b4b018028
result_sha: f3ae3a6e3669ff6be3b5ec844b41f807a673c1da
created_at_utc: 2026-10-16T07:53:18Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4 — résultats acquis, correction du quantum et blocage causal

**Résultat technique : BLOCKED pour la campagne complète R2 sur cible.** Le statut
provisoire IN_PROGRESS ci-dessus concerne uniquement la publication/relecture de
ce bilan. La correction logicielle et sa récupération physique sont démontrées ;
START/RENEW/STOP/APPLY en coexistence physique complète ne sont pas qualifiés.
Le firmware initial est restauré intégralement et la FRAM historique est préservée.

[Architecture](../../05_Firmware/LIVE_R4.md) · [Preuves et manifeste](preuves/TR2-20261016-R4/README.md).
Code effectivement évalué : `f3ae3a6e3669ff6be3b5ec844b41f807a673c1da`. Mission et règles ChatGPT inchangées.

## 1. Livraison logicielle et invariants

- FRAM physique par commandes complètes de 16/32/64 octets testées ; q32 sur cible.
  Contexte coopératif explicite partagé R1/R2, CRC, SHA et recovery incrémentaux,
  callbacks non réentrants, erreur sticky et arrêt indépendant du journal.
- Formats R0/R1/R2, ordre des barrières, générations, idempotence/anti-rejeu et
  slots STOP126/RENEW127 conservés. Pas de journal delta ni de migration de format.
  Ownership vérifié après chaque yield avant chaque écriture LIVE physique.
- Pilote FRAM existant réutilisé avec interface optionnelle <=64 octets,
  timeout HAL2ms après correction de la quantification ; V1 reste à10ms.
  CS fermé, WRDI tenté, aucune retry ajoutée. SPI1 configuré2,5MHz et SPI3 4MHz.
- Deux copies CRC et décodages d'une entrée journal auparavant enchaînés sont
  maintenant séparés par des points de service. Limite de travail1000us inchangée.
- Probe explicite `TR2_STM32_R4_BENCH=ON`, WAIT RAM, UID/horloges/SHA vérifiés,
  aucune entrée serveur production/SD/RS485. Mode3 de reprise strictement readonly,
  propriété refusée et aucun redispatch. B3 sans provider calibré demeure indisponible.

Commits logiciels : ae3d52f (socle),0e5f495 (probe),f870df0 (timeout HAL),
12e7a1f (premier mot distinct du gap),64f7bd4 (ordre init/reprise R1),f3ae3a6
(découpage copies journal et récupération readonly). Les checkpoints3bef99b puis
b9fa9e6 ont été poussés et relus intégralement ; la publication finale suit ci-dessous.

## 2. Validations réellement exécutées

Après la dernière correction fonctionnelle :

- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`
  depuis la racine : **136/136 PASS,74,52s**, cross-build ARM PASS.
  Preuve `slot-split-full-validation.log`. Reprise complète justifiée par la modification
  du store partagé, non une répétition de code inchangé.
- ASan/UBSan : configuration LIVE, moteur R2, coopération FRAM/FIFO et work,
  **4/4 PASS,141,52s**, `slot-split-sanitize-test.log`.
  Les tests FIFO/premier-mot/pilote HAL avaient également réussi après leurs corrections.
- **1561 oracles R2 de coupure réutilisés sans affaiblissement** avec coopération,
  chaque phase et frontières header/root, payload stride4096 ; pas une coupure à
  chacun des52298octets. START/RENEW/STOP/APPLY laissent262144octets modèle identiques
  à l'ancien chemin. Défauts CRC/SPI/ownership/budget/réentrée et non-dispatch conservés.
- Modèle final q32 : **624,225411s simulées,13859117XYZ,13renouvellements,
  39033 réponses RTU, FIFOmax19/512, RXqueueMax8/64, overflow0**.
  Workmax544us, servicemax320us, gap436us pour sa séquence courte.
  FC03 B0 et FC10 maximal255octets refusé02 alternés, timing fil115200/11bits,
  drain8événements/service. Latence enqueue FC10max25094us inclut réception.
  Urgence272us modèle ; contrôle négatif sans coopération déborde.
  Hypothèses SPI/CPU explicites dans LIVE_R4 ; ces nombres ne sont pas des WCET STM32.
- Canonique V1 : text172580/data480/bss602232 ; BIN SHA
  `6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9`,
  **inchangé** malgré ces ajouts optionnels. ELF actuel SHA
  `6ecbb00582e01cb943fcfc0eea2d09f56b8ed6f6b78b7738d48eca6e87865c99`.
  Le probe physique final est un autre artefact, identifié dans `bench5-program-command.json`.

## 3. Préflight et campagne physique, sans mélange des essais

Utilisateur confirme TR2-001 inchangé. ST-LINK003500463235510B37333439,
UID004A003D/38425018/20333157 vérifiés. Flash initiale2Mio et FRAM262144octets
lues deux fois concordantes avant flash. Image héritée réellement trouvée :
M016 stockage tentative2,62312octets, SHA00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d.
Historique génération12 ; LIVE[122880,245760) et tail initialement entièrement0.
Allocation normative disjointe, owner unique, sauvegarde vérifiée et SHA complet
recontrôlé sur cible avant propriété ; reprise des seuls checkpoints R4 connus,
jamais un booléen permissif sans ces vérifications. Aucun write hors LIVE.

| Étape | Observation et conclusion limitée |
|---|---|
| bench1 | Timeout HAL1ms après256octets ;0write. ErrorCode32 et comparaison HAL >=Timeout expliquent le risque au prochain tick. Option2ms corrigée, sans changer V1. |
| bench2 | SHA FRAM complet conforme ; FIFO vide avant5ms, arrêt sûr,0write. |
| bench3 readonly | Premier burst11,043ms depuis appel local START ;180826XYZ/22604TS, FIFOmax18, gap601us, readFRAMmax132us/service461us/arrêt235us. Aucun défaut de ce préfixe. Le halt GDB a interrompu le contrôle FRAM final à l'arrêt, provoquant un timeout : **RUN entier non PASS**. Contrôle indépendant ensuite :262144octets initialement identiques. |
| bench3 R1/R2 | Migration puis APPLY config2 réussis,104852octets écrits dans LIVE ; quantummax670us. Harness refusé ensuite : store R2 raccordé après mount. Corrigé en liant le contrôle avant mount ; R1 réussi non répété. Snapshot R1 :414octets différents, historique/tail identiques. |
| bench4 R2 | START durable engagé ; dépassement **1030us>1000us** avant START confirmé, arrêt sûr236us, statut9 ;0XYZ, owner.run_id0 à l'observation.104852octets supplémentaires écrits ; cela ne constitue pas une preuve autorisant à décider rétroactivement l'effet de START. |
| attribution readonly | Dépassement1030us reproduit dans `validate_control`, checkpoint à la frontière d'entrée journal, pile GDB conservée dans `bench4-readonly-mount-attribution-ex.log`. Copies CRC/décodages trop groupés : points de service ajoutés, limite non relevée. Aucun write. |
| bench5 récupération corrigée | **Terminal10/erreur0**,6,275s,923428octets lus,0écrit, quantummax907us, service4us ; statut9/indeterminate=true/has_active=true conservés, capteur arrêté. Protégés identiques et FRAM intégrale identique au snapshot après bench4. **PASS récupération readonly uniquement**, pas reprise de la campagne R2. |

Le paramètre premier-mot20ms du probe est expérimental :
[ST Ton10ms typique](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), sans maximum garanti.
Le défaut de l'API reste l'ancien max_gap ; gap5ms, statut/overflow et surveillance
après premier mot sont inchangés. Les essais ont été repris seulement après erreur
ou correction justifiée, pas pour confirmer à nouveau un succès inchangé.

À la reprise après interruption, un retour au WAIT du probe a été observé entre
la première lecture terminale et la sauvegarde suivante ; cause non établie.
CSR1c004400 est une lecture de drapeaux sticky, pas une attribution de ce reset.
Les diagnostics GDB préparatoires refusés (frame/symbole, Python indisponible,
arrêt dans appel GDB) restent dans les preuves ; pas de succès fictif.

## 4. Blocage réel et critères restant à démontrer

Le journal contient **STARTED**, sans reçu d'effet causal permettant une conclusion
terminale. [R0 migration](../../01_Specification_source/LIVE_R0/MIGRATION_FRAM.md)
§recovery exige « pas de redispatch ; recovery9 si effet antérieur indécidable ».
Le moteur le respecte : autre transaction non terminale refusée, pas de RENEW pour
contourner l'indétermination. Aucune procédure autorisée de réconciliation/effacement
n'est prévue dans cette mission. AGENTS.md §§8–9 et R0 interdisent d'inventer un
reçu, d'écraser LIVE ou de modifier la norme pour finir le test.

**Décision structurante attendue de ChatGPT/utilisateur :** définir une mission de
maintenance/requalification traitant cette trace après archivage intégral, avec
règles causales explicites ou reprovisionnement LIVE expressément autorisé.
Ne pas modifier silencieusement les formats ni déclarer absence d'effet. Ensuite,
reprendre START/RENEW/STOP/APPLY et service prolongé sur cible avec la correction,
sans refaire les tests logiciels inchangés.

Limites conservées :

- Coexistence **écriture FRAM R2 + FIFO** non qualifiée physiquement : arrêt avant
  acquisition confirmée. Les preuves readonly FIFO et récupération ne la remplacent pas.
- 907us laisse93us sous le seuil dans ce seul chemin mesuré, sans IRQ UART/FIFO
  simultanées ; pas une borne tous états, ni budget physique production accordé.
- HAL SPI3 peut attendre20ms par appel ; HAL FRAM2ms et actions d'arrêt doivent encore
  être bornés en faute avec tick/IRQ/cleanup réels. Réserve modèle106mots insuffisante
  pour prouver arbitrairement ces fautes ; admission production reste absente/refusée.
- T1000 n'énumère que COM3/ST-LINK VCP ; pas d'adaptateur RS485 détecté. Aucun essai
  électrique RTU, aucune transmission HP/COM7. RTU mesuré ici seulement sur modèle.
- DWT à160MHz configurés mesure le MCU ; fréquences SPI déduites des RCC/prescalers,
  pas mesurées électriquement ni calibrées indépendamment. Provider temps capteur
  absent, B3 LIVE invalide et protocole2 non annoncé.
- Les API restent synchrones avec yields internes. Une future intake RTU2 immuable
  et différée doit éviter la réentrée du même serveur. Latence commande durable,
  WCET en faute, CPU total et pile high-water restent non qualifiés.

Si les bornes restent impossibles, évaluer un journal delta comme **décision
structurante distincte** ; il ne corrige pas à lui seul les timeouts SPI3 et ne
justifie aucune migration silencieuse. R5 peut avancer sur interfaces gelées ;
R6/LIVE physique restent fermés aux conclusions prématurées.

## 5. État matériel final et restauration vérifiée

**PASS restauration à07:50:12UTC le16/10/2026.** Programmation standard de114688octets
(14pages8Kio) dérivés de la sauvegarde initiale : uniquement l'étendue modifiée par
les probes, padding FF inclus, aucune mass erase ni OptionBytes/RDP.
CLI vérifie, puis GDB relit les **2Mio complets**, strictement identiques au backup :
SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.
L'ancien firmware **M016 commissioning** attend, GDB détaché ; ce n'est pas une
nouvelle qualification de M014-H ou d'un firmware cumulatif LIVE.

FRAM après récupération puis restauration relue et identique au snapshot après R2 :
SHA `c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561`.
Seulement566octets diffèrent du tout premier état, tous dans LIVE. Historique
[0,122880) et tail[245760,262144) restent identiques ; trace STARTED conservée.
SHA historique4e3c2d39f03eaf7cf46d8c987d944548c1f14752d36bafa1a41439078cfb3cf3,
tail4fe7b59af6de3b665b67788cc2f99892ab827efae3a467342b3bb4e3bc8e5bfe.
Aucun effacement FRAM pour restaurer, aucun essai SD ni write SD R4, aucun RS485.
Backups binaires restent locaux : `/home/lolo/dev/msm/reports/TR2-r4/hardware/`.
Les SHA publiés ne prétendent pas remplacer ces contenus.

## 6. Architecture, Git et publication

421modules/interfaces/tests inventoriés ; àf3ae3a6,32868lignes C/C++ propriétaires,
47665tests, soit+527/+336 depuis66c5e16. Méthode splitlines exacte, vendor/build exclus,
pas additions Git. Work64octets ARM ; acquisition1104/FIFO1072 ; moteur488/store9656,
config584/média96. Probe final text106212/data476/bss593928.
`.su` = cadres statiques O0 ARM ; aucune pile maximale physique inventée.
Réutilisation parser/transport/FRAM/accumulateur, aucune suppression de code déclaré mort.

Dépôt historique `MSM_vibe` propre et conservé à82169af7e76797731358852bec83bfb0ceb54ccf.
Travail isolé `MSM_vibe-R4-cooperative-fram`, branche `codex/r4-cooperative-fram`,
push normal explicite versmain autorisé.53worktrees ; autres HEAD préservés.
Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 et
f3b829e6188f491920edf74a5f9ed0f559d08b87 préservés ;2336fichiers protégés inchangés,
archives R3-B mission/rapport byte-identiques à66c5e16. Aucun reset/merge/rebase/force.
Journal et roadmap actualisés : résultats bornés, restauration, blocage et prochaine décision.
Fins CRLF et espaces des sorties Windows brutes conservés pour traçabilité ; diff
code/documentation contrôlé séparément des logs. Aucun secret ajouté.

Publication finale et relecture intégrale GitHub en cours ; statut technique BLOCKED
ne signifie pas code non sauvegardé. Le SHA final de publication et l'état Git seront
fournis dans la réponse après vérification distante. Contrôle indépendant ChatGPT distinct.
