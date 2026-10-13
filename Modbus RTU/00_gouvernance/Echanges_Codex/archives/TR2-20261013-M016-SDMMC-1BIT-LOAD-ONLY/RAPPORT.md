---
mission_id: TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY
status: FAILED
base_ref: main
base_sha: 3538017a361d7178a1ac19a9563aeaf2eae1af37
initial_head: 5efffbd84950cbe96d114b0619fd8846b07d63f0
result_sha: c6c56b63e02cb78f176f278195dc114a6b7cbe76
created_at_utc: 2026-10-13T22:12:17.713173+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — résultat du seul essai SDMMC2 1 bit LOAD-ONLY

**Résultat technique : ÉCHEC du scénario de 600 secondes.** Une seule charge a été déclenchée le **13/10/2026 à 22:02:59 UTC** (14/10 à 00:02:59 à Paris). Arrêt après **4,164454 s**, premier refus `load_stage=99 / load_error=3` : **débordement du ring SRAM de 512 KiB**. Les **1 310 720 octets effectivement écrits** ont ensuite été intégralement relus et comparés : **zéro différence**, aucune nouvelle écriture ni répétition de charge. Les dix minutes ne sont pas validées.

**Statut final FAILED** : le scénario physique n’a pas satisfait ses critères. Le code, les résultats et les preuves ont été publiés sur GitHub, puis intégralement relus ; ce succès de publication ne qualifie pas les dix minutes.

## Tranches réalisées et code évalué

1. Inspection Git, gouvernance et références H3h-A, H3h-C2c, P8-B, banc ST et rapport GPIO. Dépôt principal conservé ; travail dans `MSM_vibe-M016-1bit-load-only`, branche locale `codex/m016-1bit-load-only`, publication normale sur `main`. Le `base_sha` du contrat est un repère historique, distinct du HEAD initial.
2. Profil indépendant ajouté dans `experiments/sdmmc_st_reference` : option CMake `BENCH_LOAD_ONLY`, `src/load_main.c`, [scénario et commandes de build](../../../experiments/sdmmc_st_reference/LOAD_ONLY.md). HAL/CMSIS officiels CubeU5 v1.9.0 et main ST historique conservés. Code évalué **`c6c56b63e02cb78f176f278195dc114a6b7cbe76`**, commit « M016 load-only checkpoint: single 600s 1-bit arrival-paced run with bounded real queue ». Aucune modification du code évalué après le flash.
3. Transition GPIO sécurisée, identité, flash standard vérifié, libération des anciennes sorties puis insertion SD hors alimentation par l'utilisateur. INIT unique sans écriture, checkpoint **`7bb3f8e17474ca1dd19b6507823bf37765441bee`**, publié et relu avant charge. L'interruption pour mauvaise manipulation était **avant RUN** ; la demande de reprise n'a donc pas conduit à rejouer un essai.
4. RUN unique, capture seulement au point terminal, puis vérification en lecture seule de tous les secteurs réellement écrits. Analyse hors ligne et publication des résultats ; aucune exploration 4 bits ni campagne annexe.

Cross-build spécialisé réussi, puis rebuild d'instrumentation du premier échec réussi ; sorties conservées. Validation limitée au projet indépendant, sans interface du runtime modifiée : pas de relance des tests hôte ou des validations acquises de production. BSS **609 152 octets** dans ce harness (RAM linker 768 KiB) ; ce n'est pas un budget RAM du firmware TR2.

BIN **23 760 octets**, SHA-256 **`2639dee58fd27da7471c513336a29bcc7493ff92a51c65b5fb9a02ac087791f1`** ; build local `/home/lolo/dev/msm/reports/M016-1bit-load-build`. Empreintes ELF/MAP dans [artifacts.json](preuves/TR2-20261013-M016-1BIT-LOAD/artifacts.json). Flash à `0x08000000` par la chaîne qualifiée, vérification CLI et readback GDB intégral concordants, reset logiciel de préparation après sortie du programmateur. Aucun flash/reset supplémentaire pendant ou après la charge.

## Cible, média et profil fixes

- Seconde NUCLEO-U575ZI-Q uniquement, ST-LINK **0032001F3235510C37333439**, UID **001B0046 / 38425018 / 20333157** vérifiés et gardés dans le code/scripts. Principal jamais sélectionné.
- Carte de test sacrifiable confirmée par l'utilisateur, CID **03534453 / 44333247 / 858BF092 / 3C01927A**, **62 333 952 secteurs de 512 octets**. Région planifiée **LBA 2048..502054**, sans formatage. Région effectivement écrite et relue **2048..4607**, vingt chunks de 128 secteurs.
- Bus **1 bit** observé côté contrôleur (`CLKCR=8`, WIDBUS=0) et carte (SSR DataBusWidth=0). Kernel PLL1 **160 MHz**, diviseur 8, fréquence calculée **10 MHz**, front montant ; fréquence établie par configuration/registres, sans mesure électrique.
- Arrivée logique **26 667 records/s × 16 = 426 672 B/s**, calculée sur DWT 160 MHz étendu ; production réelle dans un ring borné **524 288 octets**, SysTick prioritaire. Chunks **65 536 octets**, deux nodes DMA ST contigus ; tampon libéré seulement après transfert et retour carte TRANSFER/READY. Aucun retry.
- Cible prévue : **256 003 200 octets logiques en 600 s**, **256 003 584 octets physiques**, 384 octets de padding A5. Motif déterministe par offset ; records techniques représentatifs du débit, **pas des échantillons capteur ni une validation du codec P8**.

## Résultats observés et calculs

| Mesure | Résultat du seul RUN |
|---|---:|
| Durée de charge jusqu'à l'arrêt terminal | 4,164454 s |
| Octets logiques générés selon arrivée | 1 769 888 |
| Octets acceptés dans le ring | 1 769 472 |
| Octets logiques/physiques écrits | 1 310 720 / 1 310 720 |
| Octets relus et vérifiés hors compteur embarqué | 1 310 720 |
| Chunks écrits puis relus | 20 / 20 |
| Backlog maximal, transfert en cours inclus | 524 288 octets |
| Débordement / pertes à la première saturation | 1 / 416 octets (26 records) |
| Données acceptées restant en SRAM à l'arrêt | 458 752 octets |
| Débit utile écrit / durée de charge | 314 739,94 B/s |
| Latence minimale / P95 / P99 / maximale | 142,148 / 285,588 / 323,973 / 323,973 ms |
| HAL return / HAL ErrorCode / DMA error / CMD error | 0 / 0 / 0 / 0 |
| Différences sur le préfixe intégralement relu | 0 ; première divergence absente |
| Gap maximal observé entre ticks producteur | 1 000 µs |

Durée mesurée par DWT ; temps hôte **9,306 s** comprend connexion et capture et n'est pas la durée de charge. Le volume généré correspond par calcul à **4,148123 s** d'arrivée ; ce calcul n'est pas un horodatage exact du débordement. `load_generation_us=0` signifie que les 600 secondes n'ont pas été atteintes. Les pertes comptées sont celles du premier débordement, après lequel le producteur s'arrête : aucune simulation fictive des 596 secondes restantes.

Latences mesurées avant `transfer()` jusqu'à son retour, y compris commandes de statut et attente READY. Quantiles au rang supérieur `ceil(p×20)` sur **seulement vingt valeurs**, sans prétention d'endurance. [Métriques brutes et calculs](preuves/TR2-20261013-M016-1BIT-LOAD/run-analysis.json), [vingt latences](preuves/TR2-20261013-M016-1BIT-LOAD/write-latencies.csv), [log terminal](preuves/TR2-20261013-M016-1BIT-LOAD/single-load-run.log).

Le débit utile observé est inférieur aux **426 672 B/s** requis. Le ring inclut le chunk DMA en cours : il atteint 512 KiB pendant la vingtième écriture, puis cette écriture se termine, libérant 64 KiB. Le producteur a déjà enregistré l'overflow ; le consommateur s'arrête. Les 458 752 octets encore en SRAM ne sont ni écrits ni prétendus vérifiés sur SD.

## Intégrité du seul préfixe écrit

La branche d'erreur du harness retourne avant sa vérification automatique : les compteurs embarqués `load_verified=0`, `load_read_chunks=0`, `load_mismatches=0` du terminal **ne démontraient aucune intégrité**. Ils n'ont pas été modifiés pour simuler un succès.

Pour satisfaire la vérification intégrale des données réellement écrites, continuation GDB **en lecture seule**, sur la même image et le même état terminal, sans reset : vingt appels `transfer(0, rx, LBA, 65536)`, chacun réussi, dumps locaux puis comparaison indépendante Python **octet par octet** au motif attendu. SHA-256 réel et attendu concordants pour chaque chunk, zéro divergence. [Log des vingt lectures](preuves/TR2-20261013-M016-1BIT-LOAD/integral-written-prefix-readonly.log), [résultats et empreintes](preuves/TR2-20261013-M016-1BIT-LOAD/integral-prefix-verification.json), [script utilisé](preuves/TR2-20261013-M016-1BIT-LOAD/m016_load_verify_prefix.py). Ce n'est pas une seconde charge ni une mesure de performance annexe. Aucun retry.

## Première cause prouvée, limites et suite recommandée

**Fait établi : saturation de la file et pertes, avec un débit utile insuffisant dans ce scénario.** Ni erreur HAL/SDMMC rapportée, ni faute CPU (`CFSR=HFSR=0`). Les vingt écritures et lectures aboutissent ; cela ne démontre pas une absence générale de défaut matériel.

La cause détaillée des latences (carte, chemin d'attente/DMA, coût de production, interaction d'interruptions) **n'est pas isolée par ce seul essai**. Le C2c historique à 1 bit/10 MHz ne peut pas être assimilé à ce nouveau scénario : pas de relance ou diagnostic supplémentaire pour expliquer l'écart. Aucun correctif logiciel de cause racine suffisamment démontré ici ; aucun changement spéculatif appliqué.

**Action minimale justifiée pour la suite :** une mission distincte doit mesurer où le temps d'écriture est consommé dans ce chemin fixe, puis corriger uniquement une cause démontrée avant une nouvelle charge autorisée. Augmenter arbitrairement le tampon ne démontre pas que le déficit moyen serait résolu ; aucun dimensionnement de production ne peut être retenu sur cet échec.

Non qualifiés : charge de dix minutes, stockage V1 disponible, provisioning, runtime TR2, coexistence FIFO/Modbus, coupure d'alimentation, endurance de sept jours, SDMMC 4 bits. Aucun changement des spécifications gelées, de l'allocation ou des dettes A-04.

## État matériel, conservation et publication

Seconde carte laissée avec le firmware sûr **1 bit**, `stage=99/error=3`, producteur arrêté, aucun auto-write, anciennes sorties DAT1/DAT2/DAT3 libérées. Dernier état après lectures : HAL ErrorCode=0, R1=0x900 TRANSFER/READY, CFSR/HFSR=0, GDB détaché. Carte de test peut rester insérée dans cet état ; aucune restauration GPIO n'a été faite.

TR2 principal, FRAM, microSD principale et Modbus **non accédés**. Son probe stockage historique n'a pas été restauré : intervention interdite par ce contrat. Aucun mass erase, Option Bytes/RDP, formatage, transmission HP ou opération physique par Codex. Les manipulations de transition ont été réalisées et confirmées par l'utilisateur.

Dépôt original conservé au HEAD **82169af7e76797731358852bec83bfb0ceb54ccf** ; ses fichiers, les autres worktrees, checkpoints et les deux stashes **6251b988e8b20f97997a8f98fd26d4d35a45a1b2** / **f3b829e6188f491920edf74a5f9ed0f559d08b87** préservés. Rapport et mission GPIO archivés byte-identiques depuis `50c78fa` au checkpoint précédent.

[Preuves et index](preuves/TR2-20261013-M016-1BIT-LOAD/README.md), [manifest SHA-256](preuves/TR2-20261013-M016-1BIT-LOAD/manifest.json). Bruts complets locaux : `/home/lolo/dev/msm/reports/TR2-M016-1BIT-LOAD-20261013T215507Z` ; les vingt dumps SD sont conservés, leurs empreintes publiées, sans ajout de 1,3 MB de bruts au Git public.

**Journal ETAT_COURANT_TR2.md actualisé** : transition sûre, essai unique échoué, intégrité du préfixe, état laissé et prochaine décision de mission. Permissions exercées : développement, builds, commit/push normal vers `main`, flash standard et debug de la seconde cible uniquement.

Commit de résultats **`5d5d9a894f13a0a7b5b5f6bf707ca9a5a0173be5`**, message « M016 load-only: record sole run overflow and verify every written byte », push normal `HEAD:refs/heads/main` réussi. Après fetch depuis GitHub et contrôle `ls-remote`, **58 fichiers** modifiés depuis le HEAD initial ont été intégralement relus depuis cette référence et comparés byte à byte aux fichiers locaux, y compris code, archives, rapport, journal et preuves ; manifest vérifié. [Preuve de relecture](preuves/TR2-20261013-M016-1BIT-LOAD/remote-readback-5d5d9a89.json). Contrat, gouvernance, registre A-04, code de production et main ST historique inchangés ; deux stashes préservés, worktree de mission et dépôt original propres à ce contrôle.

Cette mise à jour clôt le rapport en FAILED. Le SHA du commit final contenant ce statut et le contrôle de sa publication sont fournis dans la réponse finale, sans auto-référence. Reprise technique après nouvelle mission cadrée ; aucun second RUN exécuté.
