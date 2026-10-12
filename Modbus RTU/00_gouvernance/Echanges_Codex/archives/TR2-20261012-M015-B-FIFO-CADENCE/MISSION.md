---
mission_id: TR2-20261012-M015-B-FIFO-CADENCE
status: READY
base_ref: main
base_sha: eab07d49a5f3b86506a97877d0fccf2188fb0a7a
created_at_utc: 2026-10-12T10:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-B — Qualification bornée FIFO/cadence IIS3DWB

## Périmètre volontairement modeste
Mission courte, **un seul objectif** : déterminer et démontrer sur STM32 si l'utilisation de la FIFO du IIS3DWB, en SPI mode 3 qualifié sur ce banc, permet de capturer une séquence bornée d'échantillons XYZ consécutifs avec détection explicite des pertes et cadence vérifiable. Ne pas implémenter ici DMA, pipeline continu de production, stockage microSD, campagnes B5, publication Modbus vibration, FFT ni calibration métrologique. Si FIFO impossible dans le budget, produire un audit borné documentant le verrou exact et un prototype hôte sans laisser de flash diagnostic permanent.

## Reprise et autonomie
Lire AGENTS.md, protocole V2-B3, rapport M015-A DONE, preuves M015-A, roadmap et sources réelles. Vérifier HEAD distant et local, worktrees/stashes historiques ; préserver le dépôt d'origine, le firmware permanent M014-H, identité FRAM V2 génération12 et microSD entièrement désactivée. Archiver mission/rapport M015-A suivant protocole sans écrasement. Worktree isolé sur HEAD réel de main ; aucune fusion/rebase/reset destructif ni force push. Commits/push normaux autorisés, flash/debug standard qualifiés autorisés. Aucun HOTPLUG CubeProgrammer, mass erase, Option Bytes, RDP, écriture FRAM/SD, câblage modifié ni intervention physique non automatisée.

## Tranches et critères
1. **Audit ciblé** : relire driver IIS3DWB, datasheet ST et configuration mode 3/625 kHz qualifiée, registres FIFO, format et watermark, ODR effectif, overrun et règles de lecture ; vérifier limites du bus SPI et des buffers STM32. Établir des critères quantitatifs et ne pas confondre horodatages MCU avec instants de conversion capteur. Les mouvements du voilier sont possibles : ne pas supposer l'immobilité du banc ni rejeter des valeurs physiques variables pour ce seul motif.
2. **Prototype minimal** : ajouter uniquement la capture FIFO bornée nécessaire, sans activation en production et sans dépendance au stockage. Préserver les données brutes, paramètres et statuts FIFO/overrun ; comptabiliser acquisitions, débordements, trous observables et temps de lecture. Ne pas prétendre que FIFO garantit à elle seule l'absence de perte ou l'ODR réel. Éviter les gros refactorings et l'ajout de DMA.
3. **Validation proportionnée et essai physique** : tests hôte ciblés et cross-build, capture réelle si préconditions établies ; sauvegarde/hashes, flash standard sûr, logs et preuves reproductibles, restauration octet-à-octet du firmware M014-H et contrôle B0/B1 de reprise si flash temporaire. Si les capacités ne permettent pas une preuve physique fiable, livrer la cause et les acquis sans faux succès.
4. **Publication** : publier code, tests, CSV/manifest/mesures et DERNIER_RAPPORT.md ; mettre à jour ETAT_COURANT et roadmap avec M015 IN PROGRESS et M015-B DONE seulement si critères bornés démontrés. Vérifier SHA distant et relecture des livrables.

## Communication et budget
Annoncer brièvement les tranches et l'avancement aux transitions importantes ; information non bloquante sans microvalidations. **Limiter strictement la portée** : après un audit ciblé, un prototype et une campagne de validation, ne pas multiplier les variantes SPI ni les requalifications déjà acquises. Préserver par commit/push incrémental dès qu'une tranche est validée ; si quota faible, publier immédiatement un rapport de reprise exploitable. Les permissions allow_commit/push/flash/debug sont toutes true. Escalader uniquement risque matériel non maîtrisé, opération destructive inhabituelle, contradiction normative ou geste physique indispensable. L'utilisateur délègue les choix techniques ordinaires.
