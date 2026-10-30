---
mission_id: TR2-20261030-R3-8H-FAT32-AUTONOME
status: BLOCKED
base_ref: main
base_sha: 7dcceb510862be2d6a25c2c9fa82d2a2e8741f70
initial_head: 395f9ccd5c31edeea6761455cf92ba340f984578
result_sha: 1d3a5763c34649d36b216b8ab1a1b894ae0a1210
created_at_utc: 2026-10-30T22:46:53.839183+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-8H — durée acquise et stockage intègre, qualification refusée pour saturation

**BLOCKED — P0 réalisé et publié/relu ; décision structurante P1-6 nécessaire avant modification de représentation/format ou politique qualité.** Une seule campagne longue a été exécutée. La borne intérieure indépendante atteint **28 801,0888096 s QPC**, mais une sortie FIR porte OUTPUT_CLIP dans cet intervalle. Le fichier annonce COMPLETE ; le lecteur strict le refuse correctement. Aucun contrôle n'a été affaibli, aucun second essai lancé. Ce résultat ne clôture pas R3 en succès et n'autorise aucun GO produit.

## Périmètre, reprise et image réellement exécutée

Mission sur `main`, permissions commit/push/flash/debug true. Les développements u64/v3 et l'unique flash ont précédé le lancement autonome. À la reprise, seuls les diagnostics terminaux GDB ont été relevés après arrêt ; **aucun reset, flash, START ni émission Modbus supplémentaire**. TR2-001, sa FRAM, COM4 et le service de production n'ont pas été accédés.

Cible **TR2-002**, ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**. Image physique issue de **1d3a5763c34649d36b216b8ab1a1b894ae0a1210**, profil ENDURANCE_8H/QPC/ENDURANCE_1H, BIN 54 536 B SHA-256 **7e9e9b331486c116770e1aeaa48eb0f34025c19f0a47b136f4c453bcbe51b3af**, ELF **70b3cb03321f8dc90dd1a2cd20538878a782452c289b0b8ec1058070ce3a9db4**. Flash standard et readback exact conservés dans `gate/`. Ancienne image 24 et moyen de restauration préservés ; restauration inutile pour ce banc expérimental arrêté au WAIT.

SPI3 capteur 8 MHz, SPI2 SD 4 MHz, file 64 KiB, quantum FIFO64 mots / SD128 B, groupes CMD25 1..8 secteurs et gardes work/service/gap 1000/2500/5000 µs inchangés. FIR Q15 /10 inchangé, DC conservée ; aucune adoption de cadence V1, AC1, R5 ou métrologie normative. ±8 g est le profil logique, pas un étalonnage.

Le dépôt principal historique et le worktree de lancement sont préservés. Le worktree de publication `MSM_vibe-R3-8h-E4` part du main documentaire **01e4f29f** reçu pendant les huit heures ; ses notes R5/UTC/confort sont conservées sans implémentation implicite. Checkpoints terminaux **ed7f89ec**, correctif/outils **bc0f02f0**, attente physique **fa7a8e62** déjà publiés et relus. Deux stashes historiques conservés : `6251b988e8b20f97997a8f98fd26d4d35a45a1b2`, `f3b829e6188f491920edf74a5f9ed0f559d08b87`. Aucun merge/rebase/reset/force push.

## Autonomie et durée indépendante réellement observées

START réel le **30/10/2026 à 14:01:42,726861 UTC** (annotation civile). Après le checkpoint **62c4fd1d**, Codex a terminé sa session : aucun moniteur Codex/WSL/GDB ni terminal outil laissé huit heures. Le STM32 a seul acquis, arrêté FIFO et vidé ses écritures. L'instrument Windows natif QPC était passif et non nécessaire à la progression du firmware. Alimentation ST-LINK USB vers 3,3 V, T1000 secteur sans veille automatique AC ; disponibilité neuf heures confirmée par l'utilisateur. Aucune politique Windows modifiée.

Journal QPC brut conservé dans `terminal/`, fréquence **10 000 000 Hz**, session **8444551089417146** :

- A actif, séquence822 : QPC send **12325779577523**, receive **12325780208623**, 12 464 XYZ natifs acceptés ; ACTIVE exige un premier groupe SD déjà acquitté.
- B actif, séquence823, même UID/session et source croissante : 766 225 688 XYZ acceptés. La borne intérieure, calculée entre receive A et send B, vaut **28 801,0888096 s**, sans extrapolation d'oscillateur MCU.
- Terminal séquence982 / flags1284 : 808 679 192 XYZ acceptés. RAM ensuite lue seulement après terminal : stage4/error0/clock_fault0, FIFO arrêté, file vide, writer inactif, CS haute ; digest producteur 32 B sauvegardé avant coupure, puis GDB détaché.

La garde MCU vaut **30 600 001 351 µs nominales** observées. Ce nombre n'est pas une durée SI étalonnée ; les temps source sont 64 bits et dépassent plusieurs rollovers32. Le témoin indépendant prouve la durée active, pas une absence de défaut de qualité.

## Lecture PC réelle et vérification intégrale

Après STOP/SD quiescente/détachement, l'utilisateur a confirmé **« carte insérée dans le PC, TR2 hors tension »**. Lecture ordinaire du lecteur natif Realtek du T1000, FAT32, capacité **31 914 983 424 B**, carte CID SPI conservé **0353445344333247858bf0923c01927b**, 62 333 952 secteurs. Copie exclusive des deux fichiers puis SHA-256 de chaque original sur carte et copie : identiques. Aucune écriture, réparation, suppression ni formatage sur la carte.

| Fichier | Taille originale | Contrôle |
|---|---:|---|
| E8N0003.TR2 | 131 072 B | Lecteur strict COMPLETE ; 68 records, 5 260 XYZ, 31 560 B utiles ; SHA amont RAM exact |
| E8N0005.TR2 | 671 088 640 B | Lecteur strict rejeté pour clipping ; contrôle forensique de **1 036 769 records / 80 867 920 XYZ / 485 207 520 B utiles** entièrement conforme aux octets producteur |

SHA-256 original complet E8N0003 : `0e2f5092b58f862ecde6b54bb3027c65bfc94134cd545ef5737d5ff8286d662b`.

SHA-256 original complet E8N0005 : `41c3f4454c9f224d2d470486692c2f77b23c4518f348797da0b4329e015b0432`.

SHA-256 de **tous les octets utiles** E8N0005 : **`1645c336a60f590d8cf03b69dc9b871be561e6cc9965ab779c8e13db54076a14`**, comparé au digest producteur sauvegardé séparément en RAM, pas seulement au footer du même fichier. Pour le gate : `9c6c3a6a1ee2ba88754e3324f6205e727fd13f06fd4c801d1ac5b4779321f29a`.

Header/footer v3, champs u64/réservés, CRC de chaque record, session/identité/provenance, séquence, indices, temps strictement croissants, padding, comptes et digest utiles vérifiés. Préfixe128 sorties persistées identique à la RAM, oracle numérique indépendant103 sorties préfixe exact ; **pas de recalcul FIR indépendant des 808 millions de bruts**. Les dix anciens fichiers RUN/R2N/H1N/Q1N et TR2INFO.TXT sont re-hachés inchangés.

Les copies PC intégrales sont préservées hors dépôt dans `/home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/pc-final/`. Leurs queues préallouées inutilisées peuvent contenir d'anciens octets et ne sont pas publiées. `pc/EXTENTS.json` décrit tous les octets réellement engagés : **530 826 240 B header+records** et footer512 B à l'offset671088128 pour le long ; **140 261 888 B de queue inutilisée exclus**. Tous ces vrais extents sont publiés par fragments zlib avec SHA brut/stocké. Une reconstruction à queue nulle sert au contrôle indépendant ; elle n'est jamais présentée comme le fichier PC original ni comparée à son SHA entier.

## Saturation localisée et limite du diagnostic

Le lecteur existant lève exactement **`rail or clipping in completed capture`**. L'outil distinct `inspect_quality_failure.py` conserve cette décision et retourne **REJECTED_QUALITY / measurement_qualified=false**, tout en démontrant séparément l'intégrité des octets. Il n'est pas un importeur d'acceptation alternatif.

Une seule sortie clippée selon compteur RAM/footer, zéro sortie portant SOURCE_RAIL. Un seul record OUTPUT_CLIP : **980435**, offset **501983232**, indices de sortie76473930..76474007. Unique candidat aux limites int16 dans ce record : sortie **76473993**, source décimée associée **764739930**, XYZ **[-32768, -32768, 30863]**. La qualité est agrégée par record ; le masque exact des axes clippés n'est pas archivé. Temps interpolé approximatif **30116953536 µs MCU**, soit **28937,198907 s nominales après la première source** ; aucune UTC de l'incident n'est connue. L'indice source et le temps sont entre les témoins A et B : le défaut est donc dans l'intervalle actif chronométré, pas seulement dans sa garde finale. Record, cinq records de contexte et CSV390 sorties préservés sous `pc/`.

Le FIR inchangé peut dépasser int16 sans source exactement au rail : somme absolue des coefficients **47468/32768=1,4486083984375**, pire accumulateur **1555431424 < INT32_MAX**. Un reproducteur C synthétique sur noyau réel, entrées±32766, donne47465 avant clamp32767/OUTPUT_CLIP sans SOURCE_RAIL. Cela explique la compatibilité logique `rails0/clips1` ; **cela ne prouve pas la cause physique de cet événement**. Bruts107 XYZ au voisinage de l'incident non archivés : mouvement/choc, entrée capteur, erreur SPI ou autre cause ne peuvent être départagés avec ces seules preuves. Aucun diagnostic physique supplémentaire ni modification de gain/FS/coefficients n'a été effectué.

## Mesures de fonctionnement observées

| Indicateur | Résultat long |
|---|---:|
| Native acceptés / sorties / warmup / natifs en attente | 808 679 192 / 80 867 920 / 11 / 3 |
| FIFO OVR, erreurs SD/file/temps, queue overflow | 0 |
| FIFO pic / file pic | 51 mots / 8192 B |
| Work / service / gap max | 713,3875 / 1485,66875 / 1744 µs |
| Quantum SD max | 491,61875 µs |
| Groupe SD max / busy secteur max | 241323 / 158730 µs |
| Groupes / secteurs écrits | 129597 / 1036769 |
| STOP FIFO / drain | 150 / 17566 µs |
| Pile plus profonde / plancher libre | 1776 / 2320 B |
| Contrôles horloge / résidu max | 304403 / 54 µs |
| SOURCE_RAIL / OUTPUT_CLIP | 0 / **1** |

Ces observations montrent les marges sur cette campagne et la concordance stockage, pas un WCET, une garantie de latence arbitraire ni une qualification de coupure.

## Correctif logiciel et validations conservées

Défaut de harness démontré : succès de transport entraînant footer COMPLETE sans vérifier rails/clips, alors que le lecteur exige leur absence. Commit **bc0f02f05e1ce7a67981975bfd0942dcd835fc62** ajoute, pour le seul profil8H, **après drain et avant footer**, retourerreur26 si rails/clips ; préfixe conservé INCOMPLETE, aucun faux succès. Candidat compilé, **non flashé et non qualifié sur cible**. L'image physique1d reste en Flash, arrêtée au WAIT ; aucun risque justifiant sa restauration sur ce banc éteint.

Validations déjà acquises, non répétées sur code inchangé :

- Quatre tests hôtes ciblés u64/v3/rollovers24h/FAT32 taille réelle/erreurs et gardes ; cross-build ARM exact et canonique PASS avant le gate. L'appel direct initial de tr2_validate non exécutable a échoué126 ; invocation Bash documentée corrigée sans chmod.
- Unique gate court physique v3 PASS, désormais relu intégralement sur PC ; précondition lecture FatFs+SHA du gate avant le long observée.
- Tests nouveaux du contrôle forensique PASS : rejet qualité maintenu, mauvais digest externe/réservésv3/CRC refusés, footer absent INCOMPLETE avec préfixe exact. ARM du garde qualité PASS, sans flash.
- Reprise PC : `copy_and_inspect_e8.py`, achevé sans erreur, vrais originaux/copies hachés, intégralité des enregistrements contrôlée ; aucune simulation présentée comme lecture physique.

Les logs effectifs sont dans `software/`, `gate/`, `quality-software/`, `terminal/`, `pc/`. Incident de lancement Windows initial sans capture et correction CIM native documentés dans le rapport avant START ; protections Windows inchangées, aucun contournement du diagnostic précédemment signalé.

## Livraison et suite à décider par ChatGPT

Preuves et checkpoints antérieurs préservés ; **ETAT_COURANT_TR2.md actualisé** avec fin réelle, transfert PC confirmé, intégrité complète et saturation localisée. Addendum ChatGPT reçu sur main à6bc4f24b/7f867faa : P0 avant nouvelle acquisition, puis P1 correction justifiée et P2 unique second8H conditionnel sous le même identifiant. Pas de clôture ni d’archive finale prématurée. Le premier essai reste REJECTED_QUALITY ; la mission active poursuit le même ID mais est BLOCKED sur le choix structurant P1. Les nouveaux extents ont été publiés et relus intégralement au commit1991132b. SHA final et état Git fournis dans la réponse de clôture, sans auto-référence de commit.

**État matériel final rapporté par l'utilisateur :** microSD dans le lecteur PC, TR2-002 hors tension ; dernière observation avant coupure FIFO arrêté, writer inactif, queue vide, SD quiescente, CS haute, GDB détaché. Pas de nouvelle transmission/capture, pas de reset/flash à la reprise ; TR2-001 et données persistantes de production préservés.

**Critère non satisfait :** qualité sans saturation, donc qualification globale8H refusée malgré durée et intégrité démontrées. **Décision structurante à instruire avec ChatGPT selon P1 :** revue indépendante des preuves et choix de traitement du dépassement de représentation si nécessaire. L’addendum autorise un seul second8H sous le même ID, uniquement après solution validée au court gate ; pas de campagne identique sans plan causal crédible. Ne pas changer le FIR, la plage ou les critères d'acceptation pour faire passer le fichier ; ne pas rejouer automatiquement huit heures. Coupure/reprise, FRAM/RTU, intégration produit, cadence normative et R5 restent non qualifiés.

## Reprise de publication autorisée et complément P0

Le push214370cc a transmis les objets mais a été **refusé** : main était passé de fa7a8e62 à7f867faa pendant l’envoi. Aucun succès de publication de214370cc n’est revendiqué. AGENTS§7 a été appliqué : arrêt/escalade, puis autorisation utilisateur explicite **« Oui, intégration isolée et poursuite de l’addendum »**. Worktree originalE4/commit214370cc préservés propres ; nouveau worktreeMSM_vibe-R3-8h-P0 basé7f867faa, seule tranche214 réappliquée en4f701c66. Addendum, revueA01 et roadmap ChatGPT préservés ; aucun merge/rebase/reset/force push.

P0 complément : onze **copies exclusives** supplémentaires réalisées dans `pc-historical-preserved/` : dix historiques etTR2INFO.TXT. SHA original relu = SHA copie = SHA préflight pour chacun ; copies des deuxE8 conservées danspc-final/. Cette validation porte sur les nouvelles copies demandées par l’addendum, aucun ancien essai physique n’a été rejoué. Résultats lisibles directement dans `addendum-P0/RESULTATS_PC.json`, inventaire des copies/logs sousBUNDLE. Carte toujours au PC/TR2-002 hors tension selon confirmation utilisateur.

## P1 — distinction calcul, représentation et cause physique

Lecture du code réel : accumulateurFIR **int32 déjà suffisant** (borneL1 1555431424<INT32_MAX), arrondiQ15 puis saturation explicite vers `FirDecimator10Output.xyz[int16]`, empaquetage6B/XYZ à244µg/compte dansv3. Élargir seulement l’accumulateur n’empêcherait donc pas ce clamp. Le test existant `test_fir_decimator10.c::constant_and_rails` inclut déjà un échelon±32760, vérifie les sorties clippées sansSOURCE_RAIL et le régimeDC exact : pas de relance de ce test inchangé. Le reproducteur synthétique déjà conservé confirme la possibilité, sans reconstruire l’entrée réelle absente.

Les valeurs réelles du record sont identiques aux octets hachés **avant framing/file/writerSD** ; la concordance SHA amont et compteurRAM rend l’hypothèse d’une altération introduite par le stockage incompatible avec ces observations. La cause d’entrée/mouvement/SPI reste indéterminée. LesX/Y à−32768 correspondent à−7,995392g nominaux à244µg/compte, Z30863 à+7,530572g nominaux ; ces nombres sont des conversions d’une représentation saturée, **pas des amplitudes physiques étalonnées ni des valeurs de crête récupérables**.

Conséquence : conserver simultanément FIR107/gain1/précision244µg et toute entréeint16 ne permet pas de garantir une sortieint16 sans saturation (L1>1). Réduire le gain, modifier les coefficients/bande ou passer le capteur à±16g serait un changement métrologique interdit implicitement. Une sortie plus large pourrait préserver le résultat FIR horsint16, mais exige un nouveau format expérimental et une décision explicite sur le signalement du dépassement ; elle ne réparerait ni le premier fichier déjà clippé ni une éventuelle entrée capteur erronée. Aucune évolution de format/sensibilité/qualité/API implémentée sans cette revue structurante. Pas de second8H lancé avant les portesP1/P2.

## Publication P0 vérifiée et décision de reprise

**Main1991132b9504a8ab064f306f6a125f630776b3f4**, push normal7f867faa→1991132b réussi, puis fetch/relecture complète : **293 fichiers /224 artefacts** comparés/décompressés/hachés, vendor pristine. Deux reconstructions depuis les objets de la référence GitHub : courtCOMPLETE, longREJECTED_QUALITY ; tousCRC/index/temps/comptes/SHAutile contre les digestsRAM exacts. Reçus dans `P1-decision/BUNDLE.json`. Les reconstructions à queue nulle ne sont pas les originaux PC ; leurs queues inutilisées ne sont pas prétendument connues.

Pour éviter un second transfert de295Mo, seul le processus de push appartenant à cette session a été arrêté ; origin/main était encore7f8. Les objets214 déjà conservés par GitHub ont été réutilisés via l’API Git : petits blobs/arbre/commit uniquement, **aucune mise à jour de référence par API**. Arbre serveur **b757ff1607294f07b7c77959c941d275094e594d**, strictement identique à l’arbre local415c0585 contrôlé ; parent unique7f8. Commit1991 récupéré dans un nouveau worktreeP0pub, puis **push Git normal avec négociation**,0objet retransmis. OriginalE4/214 et P0/4f701c66+415c0585 conservés, pas de merge/rebase/reset/force. Mécanismes documentés par [GitHub Git trees](https://docs.github.com/en/rest/git/trees) et [Git commits](https://docs.github.com/en/rest/git/commits) ; reçus/commandes exacts archivés. Aucun mécanisme de sécurité Windows modifié.

Le premier contrôle distant lancé avant confirmation du push a été refusé par gardeHEAD≠origin/main, sans preuve acceptée ; logs d’échec préservés. Les contrôles réussis ci-dessus ont été exécutés **après** le push1991. Validation documentaire/forensique proportionnée, aucun test acquis ni build rejoué sur source inchangée.

**Choix concret à soumettre à ChatGPT/utilisateur :** voir `P1-decision/CHOIX_REPRESENTATION.md` : garder v3/rejet et définir une instrumentation causale, ou autoriser explicitement une représentation expérimentale élargie (par exempleint32, gain/FIR/sensibilité244µg et capteur±8g inchangés), avec définition versionnée du signalement des dépassements. Cela change format et politique de qualité expérimentale : l’addendumP1-6 interdit l’intégration implicite. Pas de correction du FIR ni second8H lancés faute de cette décision ; le manque de bruts empêche une cause physique certaine. Le gardebc0f demeure compilé/nonflashé.

Journal actualisé : P0 pleinement acquis, copies historiques et publication vérifiées, blocage déplacé du geste physique au choix structurant. Mission **non clôturée** : aucune archive finale sous cetID tant que la reprise conditionnelle n’est pas tranchée. Le firmware1d reste surTR2-002 éteint, cartePC ; TR2-001/FRAM/COM4 intacts. SHA final de ce checkpoint et étatGit après son push/relecture seront fournis dans la réponse utilisateur.
