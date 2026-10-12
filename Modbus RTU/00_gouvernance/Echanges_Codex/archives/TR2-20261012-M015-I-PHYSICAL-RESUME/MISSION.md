---
mission_id: TR2-20261012-M015-I-PHYSICAL-RESUME
status: READY
base_ref: main
base_sha: 7a2aca1901162144142f3e13964a4abcc7dbed9d
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-I — Reprise de la qualification physique FIFO avec vérifications groupées

Reprise explicite de M015-I après renouvellement du quota. Lire AGENTS.md V2-B3, rapport précédent M015-I IN_PROGRESS, `Modbus RTU/00_gouvernance/REPRISE_M015_I_FIFO_GROUPEE.md`, ETAT_COURANT et roadmap sur main réel. Archiver mission/rapport antérieurs avant de les remplacer, préserver worktrees/stashes et toutes preuves. Ne pas redévelopper le harness déjà validé. SHA logiciel de référence `6c504a9d4035c2922d186a03350963bdac33379b`, BIN SHA256 `760aab56cf32aa458c861bf5f22e2801dd595f3bd73ad6a98602c1cff0cdd258`, ELF SHA256 `7893fda8b3e43e215cbab0a1bcc515d51b0a6986cdf9ff742974e53b8cbb076e`. Vérifier code et hashes avant utilisation. Le HEAD réel initial de main était `7a2aca1901162144142f3e13964a4abcc7dbed9d` avant publication du présent contrat ; relever le HEAD après publication.

## Objectif prioritaire
Exécuter la **seule campagne physique bornée** prévue en M015-I pour savoir si les contrôles SPI groupés permettent enfin de franchir la configuration et lire de vrais échantillons FIFO XYZ. Garder le fail-closed : WHO, CTRL1=A8, IF_INC, contrôles FIFO, HAL/ErrorCode/compteurs/CS et intégrité stricts. Aucun vote, substitution ni acceptation d'une valeur incohérente. Une réussite courte ne qualifie pas le flux continu V1. Pas de recherche exhaustive de la cause 0x50, ni de nouveau harness exploratoire.

## Exécution autonome
1. Préflight git/main/worktrees/stashes, matériel STLINK `003500463235510B37333439`, UID STM32 `004A003D3842501820333157`, image permanente M014-H, SDK et outillage ; vérifier que les sauvegardes, scripts et binaires référencés existent réellement. Préparer et relire les helpers G adaptés à I et wrapper HP `physical-m015-i-restored` conformément au point de reprise ; ne jamais lancer l'ancien helper G inchangé. Tests/compilations déjà verts : ne répéter que si changement de code ou doute documenté.
2. Sauvegarde **fraîche** Flash2 Mio et RAM utile, avec hashes et vérification. Avant flash, établir qu'il reste assez de temps et d'accès pour une restauration garantie. Aucun geste utilisateur, changement de câblage, de capteur, FRAM, SD ou partition. Un seul trigger `0x4D313546`, maximum deux bursts128/256 mots RAM si tous les contrôles passent. Capturer traces SPI brutes, état de configuration, FIFO/tags/XYZ/erreurs/timing et toute divergence, sans fabriquer de mesure. Stopper au premier écart bloquant.
3. Flash standard NORMAL à 0x08000000 avec verify/reset, **jamais HOTPLUG**, mass erase, Option Bytes/RDP ni effacement non requis. GDB reset/readback qualifié après dernier CLI ; debugger détaché pendant campagne. Sauvegarder preuves RAM avant reflash. Restaurer **en finally** l'image M014-H et comparer Flash2 Mio octet par octet au backup frais. Reprise HP deux FC03 B0/B1 seulement, sans GDB pendant trafic, journal entier récupéré/hashé ; contrôler FRAM V2 gen12, config UNAVAILABLE, SD OFF, IRQ49/66, fault0, MCU running. Si impossibilité de garantir restauration, ne pas flasher.
4. Publier bruts utiles, hashes, tests éventuels, diagnostic factuel, résultat positif ou BLOCKED, ETAT_COURANT, roadmap et DERNIER_RAPPORT.md ; commits et push normaux sur main, puis **relecture distante indépendante des livrables**. Conserver copies brutes locales et preuve de restauration. Aucun deuxième essai non prévu.
5. Si groupé échoue encore : ne pas remplacer physiquement le breakout de manière autonome ; documenter exactement le point de refus et préparer une proposition de comparaison A/B avec le second IIS3DWB, qui nécessitera intervention physique de l'utilisateur. Si échec reproduit sur les deux modules lors d'une mission ultérieure, proposer un firmware SPI minimal reproductible pour assistance STMicroelectronics, sans bloquer la recherche d'un contournement.

## Invariants et conditions d'arrêt
ODR V1 26667 Hz, fenêtres4096..32768, continuité/store-and-forward, mapping gelé. Ne pas confondre 256 mots RAM avec V1 continue. Pas de DMA/FFT/Web/Modbus vibration ni instrumentation nouvelle. Toute incertitude matérielle ou impossibilité de restaurer arrête la campagne avant flash ; documenter précisément. Autorisations permanentes commit/push/flash/debug appliquées sans validations répétitives ; escalade uniquement intervention physique, risque matériel incontrôlé ou décision structurante.
