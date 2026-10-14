---
mission_id: TR2-20261014-P12H3H-SDMMC1-CABLE10CM-COMPARISON
status: FAILED
base_ref: main
base_sha: 7271e03e513e3fb5a9ba33051713307125c60bbe
initial_head: e4630199d4859e09b431ea159bfa36e553589617
result_repository: https://github.com/VILLO88/NUCLEOU575
result_sha: 1e3c9183bad023ebf7d234134823d9270a0e8b64
created_at_utc: 2026-10-14T10:58:28.618028+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# SDMMC1 — comparatif ciblé fils 20 cm / 10 cm

**Résultat technique : FAILED — amélioration non démontrée.** Avant le premier debug, Windows indique déjà « No Media ». Après l'unique récupération initiale autorisée, le média reste indisponible et le STM32 présente des erreurs de commande. **Zéro cycle USB de données, zéro écriture, zéro reflash** ; un reset logiciel seulement. La campagne courte ne peut pas être engagée dans cet état. Aucun nouvel essai après le critère d'arrêt. Le statut final FAILED constate l’absence d’amélioration démontrée selon le contrat ; il ne déclare pas le contrôleur SDMMC1 défectueux.

## Tranche 1 — préservation et configuration identique

Mission et gouvernance récupérées sur main `e4630199d4859e09b431ea159bfa36e553589617`. Dépôt d'origine propre mais historiquement divergent, HEAD `82169af7e76797731358852bec83bfb0ceb54ccf` : préservé sans pull, reset, merge ou rebase. Nouveau worktree `MSM_vibe-SDMMC1-cable10cm`, branche `codex/p12h3h-sdmmc1-cable10cm`, basé sur main courant. Les worktrees existants et les deux stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` / `f3b829e6188f491920edf74a5f9ed0f559d08b87` sont conservés. AGENTS, protocole et gouvernance inchangés depuis le rapport précédent ; journal relu intégralement.

Fils Dupont **10 cm au lieu de 20 cm**, raccordement et SB19/SB22 conservés, carte insérée, ST-LINK et USB natif connectés : **déclarations utilisateur consignées dans le contrat**, pas une mesure de longueur/continuité effectuée par Codex. Aucune intervention physique redondante demandée.

Seconde cible strictement vérifiée via GDB : ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**. Carte CID **03534453/44333247/858BF092/3C01927A**, capacité enregistrée **62333952 × 512 = 31914983424 octets**. Ces valeurs proviennent du handle initialisé ; elles ne prouvent pas l'accessibilité actuelle du média.

**Readback Flash intégral des 31437 octets de l'image** byte-identique au BIN Release conservé : SHA-256 **2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a**. ELF SHA-256 **511322f3bcfc2a19b0572d5db34d86dfc53ce6eb055abd502ae8b07bbc077494**. Source tiers toujours propre et épinglée au commit `1e3c9183bad023ebf7d234134823d9270a0e8b64`. Aucune modification, rebuild ou flash. Le `result_sha` désigne le code externe évalué, pas le commit MSM de rapport.

**Configuration effective inchangée**, observée avant et après récupération : `SDMMC1_CLKCR=0x4002`, donc largeur contrôleur 4 bits/div2. RCC CCIPR1 ICLKSEL=0 et CCIPR2 SDMMCSEL=0, HSI48 ON/RDY, donc kernel 48 MHz et SD **12 MHz calculés**. CPU 160 MHz. Valeur côté carte 4 bits déjà démontrée par SSR dans le PoC précédent ; pas de nouvelle commande SSR dans cette mission. HAL, TinyUSB, broches et carte inchangés ; pas de mesure électrique de fréquence.

## Tranche 2 — état initial, récupération unique et arrêt

| Observation | Résultat réel |
|---|---|
| Windows avant tout debug, 10:55:09 UTC | USB 0483:572B / serial123456 présent et OK ; disque STM32 SD Card numéro2, Size0, **No Media** |
| GDB initial | UID/image/CID/configuration conformes ; `sd_ready=1`, HAL READY, ErrorCode **0x3005**, RESP1 **0xB00**, STA **0x81000**, USB connecté/config1, ejected=false, CFSR/HFSR0 |
| Windows après premier debug | No Media persiste, aucune écriture ni lecture de données demandée |
| Récupération initiale | **Un seul `monitor reset` GDB**, procédure déjà documentée ; detach, attente huit secondes, image originale et paramètres inchangés |
| GDB après récupération | UID/CID/CLKCR/RCC conformes ; ErrorCode **0x2005**, RESP1 **0xB00**, STA **0x81000**, HAL READY, sd_ready1, USB connecté/config1, faults0 ; detach |
| Windows final, 10:55:47 UTC | USB reconnu, mais disque Size0/**No Media** à nouveau ; garde de disponibilité non satisfait |

Codes interprétés à partir du header réel `stm32u5xx_ll_sdmmc.h` tiers : `0x3005` = CMD_CRC_FAIL `0x1` + CMD_RSP_TIMEOUT `0x4` + COM_CRC_FAILED `0x1000` + ILLEGAL_CMD `0x2000`. Après reset `0x2005` conserve CMD_CRC_FAIL + CMD_RSP_TIMEOUT + ILLEGAL_CMD. **Ce n'est pas un RX_OVERRUN (`0x20`)**. RESP1 `0xB00` encode état carte DATA5 avec READY_FOR_DATA ; HAL READY et sd_ready1 ne prouvent donc pas la disponibilité. Les erreurs sont accumulées dans le handle : aucune chronologie de première commande fautive n'est déduite de ces seuls bits.

**Arrêt contractuel atteint : support toujours indisponible après la récupération initiale.** Aucun cycle de 16×8192, aucune nouvelle écriture ni relecture de payload ; l'outil aligné original est conservé mais non exécuté. Aucun cycle2/3, aucune autre récupération, aucun diagnostic annexe. Les opérations automatiques Windows/USB du firmware original restent possibles ; « zéro écriture opérateur » ne prétend pas instrumenter toutes les requêtes du PC.

## Comparaison avec 20 cm et limites

| Critère | PoC 20 cm précédent | Comparatif 10 cm actuel |
|---|---|---|
| Image / configuration | Release originale, 4 bits, 12 MHz | Identiques, image vérifiée sans flash |
| Disponibilité | Succès bornés après récupérations, CRC et No Media récurrents | No Media initial et persistant après un reset |
| Jeu B 128 KiB write/flush + read | Intégrité intégrale, zéro différence | Aucun cycle possible |
| Durée write+flush | 3178,7446 ms (~41234 B/s applicatifs) | Non mesurée |
| Durée read | 187,4372 ms (~699285 B/s applicatifs) | Non mesurée |
| Amélioration fiable démontrée | Non qualifié | **Non démontrée** |

Les durées précédentes sont hôte/USB, pas un débit SD intrinsèque. Aucun pourcentage de gain ni comparaison de performances n'est calculable ici. Ce résultat **ne prouve pas que les fils courts sont sans effet** ; il prouve qu'ils n'ont pas suffi à rendre le média disponible dans cette observation et avec cette récupération. Pas de qualification industrielle, cadence TR2 426672 B/s, endurance, filesystem, récupération ou adoption du firmware tiers.

**Limites causales :** aucune mesure électrique, état de carte potentiellement hérité, reset logiciel distinct d'une coupure d'alimentation, effets possibles des halt/detach sur USB/polling. No Media est observé avant le premier GDB de cette mission, mais aucune cause initiale précise n'est isolée. Aucun correctif multi-read/MSC, changement de fréquence, HAL, carte ou câblage n'a été appliqué pour masquer l'échec. Une nouvelle campagne/correction exige une mission distincte ; pas de manipulation supplémentaire demandée dans celle-ci.

## Tranche 3 — publication et état laissé

[Preuves et méthode](preuves/TR2-20261014-SDMMC1-CABLE10CM-COMPARISON/README.md), [résumé](preuves/TR2-20261014-SDMMC1-CABLE10CM-COMPARISON/campaign-summary.json), [image](preuves/TR2-20261014-SDMMC1-CABLE10CM-COMPARISON/image-verification.json), [état final Windows](preuves/TR2-20261014-SDMMC1-CABLE10CM-COMPARISON/host-after-initial-recovery.json). Bruts et readback Flash local : `/home/lolo/dev/msm/reports/TR2-SDMMC1-CABLE10CM-20261014`. Sources/image tierces non redistribuées ; scripts d'outillage et petits logs publiés avec manifest SHA-256.

Firmware Release original SDMMC1 laissé sur seconde Nucleo, débogueur détaché. Dernier état MCU observé ErrorCode2005/RESP1B00 puis Windows No Media ; absence de télémétrie continue après detach. Carte de test et deux câbles connectés selon le contrat ; aucune modification persistante sensible. **TR2-001 principal, FRAM, média512Go, Modbus, Option Bytes et protections non accédés**. Pas de formatage, changement de politique Windows, ACL ou élévation.

Validation proportionnée : inspections Git/source/diff, vérification d'image par readback et observations physiques ci-dessus. **Aucun build ni ancien test rejoué** puisque le firmware et les interfaces de production sont inchangés. Seuls rapport, preuves minimales, archives et journal technique changent ; pas de tests de code nécessaires pour cette publication documentaire.

Rapport et mission du PoC précédent archivés byte-identiques depuis `7271e03e513e3fb5a9ba33051713307125c60bbe` sous `archives/TR2-20261014-P12H3H-SDMMC1-REFERENCE-POC/`. Journal `ETAT_COURANT_TR2.md` actualisé avec la comparaison FAILED, le reset unique, zéro cycle et l'état laissé. Gels, roadmap, detteA04 et règles de gouvernance inchangés. Permissions exercées : lecture/GDB/debug, un reset logiciel, documentation et commit/pushmain ; `allow_flash` non exercée. Push normal sur `main` réussi : commit `309c8654f7b8f54dedf10d958c21c885fe247e64`, message `P12 H3h SDMMC1: record bounded 10 cm cable comparison and persistent No Media`. Référence récupérée de GitHub, **33 fichiers relus intégralement et comparés byte-identiques**, **28 empreintes du manifest vérifiées** ; [preuve distante](preuves/TR2-20261014-SDMMC1-CABLE10CM-COMPARISON/remote-readback-309c8654.json). Archives et fichiers protégés contrôlés, dépôt original propre et stashes inchangés. Finalisation documentaire FAILED uniquement, aucun nouvel accès matériel ; worktree de mission propre après commit. La dernière publication sera également vérifiée ; son SHA final sera transmis dans la réponse, sans auto-référence.
