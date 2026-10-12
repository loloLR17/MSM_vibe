---
mission_id: TR2-20261012-M015-A-ACQUISITION-BRINGUP
status: READY
base_ref: main
base_sha: 762c459839439f7d27b42c11d12e309e65622a7f
created_at_utc: 2026-10-12T08:55:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-A — Bring-up physique de l'acquisition vibratoire IIS3DWB

## Gouvernance, point de reprise et autonomie
Lire AGENTS.md, protocole V2-B3, roadmap, état courant, rapport M014-H DONE, gels V1 et sources sur main courant. Vérifier HEAD, branche, état local et distant ; préserver dépôt original main local 82169af, deux stashes historiques et worktree M014-H, sans pull/rebase/reset destructif. Créer un worktree isolé depuis le HEAD distant si nécessaire. Archiver M014-H mission et rapport immuablement selon protocole, sans écrasement. M014 est clôturée **uniquement** pour B0/B1, polling et perte/reprise bornés ; préserver le firmware permanent de production actuellement actif, son BIN SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, les backups et les preuves. Les validations 113 firmware et 539 .NET acquises ne doivent pas être répétées sur code inchangé.

## Communication obligatoire pendant exécution
Annoncer les tranches prévues puis, aux transitions significatives, décrire succinctement tranche en cours, travail effectué, résultat, difficulté et prochaine étape. Informations suffisamment substantielles, non bloquantes : continuer en autonomie sans attendre d'approbation intermédiaire. Ne pas afficher le détail de chaque commande ni gaspiller les tokens. Publier des commits incrémentaux validés, vérifier les pushes distants ; ne pas attendre l'épuisement du quota pour préserver les acquis.

## Objectif M015-A
Établir une **chaîne d'acquisition vibratoire physique crédible** depuis IIS3DWB réel (WHO_AM_I 0x7B déjà observé) vers des échantillons numériques horodatés/qualifiés côté STM32, en distinguant capteur identifié, échantillons bruts réels, cohérence temporelle et données vibratoires exploitables. Ne pas annoncer M015 DONE tant que ses critères physiques ne sont pas satisfaits. Cette tranche n'implique pas M016 publication Modbus ni M020 Web.

## Lots prioritaires
1. Auditer le chemin d'acquisition réel : IIS3DWB SPI3/CS PC9, alimentation, initialisation, ODR, pleine échelle, BDU, DRDY/FIFO, conversion d'unités, timestamps, calibration et axes selon documentation constructeur et spécifications gelées. Inventorier drivers/services/tests existants ; réutiliser plutôt que dupliquer. Confirmer qui détient SPI et les IRQ ; aucune hypothèse de câblage ou signal physique inventée. Établir plan d'essai repos puis variation physique contrôlée sans solliciter l'utilisateur tant qu'une excitation automatisable sûre existe.
2. Développer le plus petit raccordement production nécessaire pour lire de vrais échantillons XYZ sur STM32 : initialisation explicite, registre de configuration readback, contrôle WHO_AM_I, status/data-ready ou FIFO selon disponibilité vérifiée, lecture cohérente, cadence/timestamps, conversions et gestion des défauts/invalidité. Pas de faux échantillons, pas de READY ou STORAGE_AVAILABLE artificiels ; configuration FRAM actuelle structurellement valide mais opérationnellement UNAVAILABLE sans stockage, à préserver. Si la politique actuelle interdit toute acquisition tant que storage indisponible, identifier précisément la dépendance normative et proposer une voie de diagnostic acquisition isolée, bornée et sans écriture, plutôt que contourner la gouvernance.
3. Tests hôte ciblés, cross-build ARM et régressions proportionnées, essais de lecture réels par GDB/diagnostic sûr ou télémétrie qualifiée. Si flash nécessaire : backup, hash, identité cible, procédure de restauration et flash standard qualifié uniquement ; aucun HOTPLUG CubeProgrammer, mass erase, Option Bytes, RDP, reformat FRAM/SD. Aucune écriture FRAM de provisioning supplémentaire sans justification ni plan de restauration. Préserver/restaurer le firmware permanent M014-H après tout profil temporaire de diagnostic, avec lecture physique de contrôle B0/B1 si nécessaire.
4. Démontrer échantillons distincts, plausibilité et évolution temporelle sur cible ; capturer logs bruts, échelles, conversions, ODR observé, saturation, erreurs et limites. Ne pas prétendre à calibration métrologique ni réponse vibratoire prouvée si aucune excitation physique n'a eu lieu. Si une action manuelle (vibration contrôlée, câblage) est indispensable, préparer protocole simple et demander uniquement ce geste, en poursuivant les travaux indépendants.
5. MicroSD **hors chemin critique M015-A** : entièrement désactivée et aucune écriture ni allocation, partition LBA65536 préservée. Si stockage nécessaire à l'activation opérationnelle, distinguer acquisition physique de diagnostic et acquisition de campagne ; documenter la dépendance, ne pas modifier silencieusement les gels V1. M013 limitations timing/DE/TC et M014 qualification bornée restent inchangées.
6. Actualiser ETAT_COURANT et ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md après preuves, notamment J2 M014 revu indépendamment et statut exact M015-A. Publier DERNIER_RAPPORT.md avec statut DONE/BLOCKED sincère, commandes, SHAs, preuves physiques et limites, restauration du profil permanent, Git final et relecture distante intégrale.

## Critères d'acceptation et escalades
DONE M015-A si lecture physique réelle multi-échantillons du IIS3DWB, provenance et paramètres prouvés, horodatage/cadence et contrôles de cohérence documentés, validation proportionnée, cible laissée en état sûr, preuves publiées. M015 global reste IN PROGRESS si excitation/vibration contrôlée ou chaîne d'acquisition complète non démontrée. Ne pas solliciter l'utilisateur pour les décisions ordinaires ; seules interventions physiques indispensables, risque matériel non maîtrisé, décision structurante non couverte ou opération destructive inhabituelle justifient escalade. Autorisations explicites allow_commit/push/flash/debug true sur main, sous garde-fous AGENTS.md. Ne pas modifier la mission active pendant exécution ; signaler divergence distante et préserver le travail selon protocole.
