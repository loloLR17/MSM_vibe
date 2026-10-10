---
mission_id: T1000-20261010-CLOTURE-001
base_ref: main
base_sha: 75eba6b431725b858db3ac5380884934026a1475
author: Utilisateur
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Contrat de clôture T1000

Source : mission utilisateur « Audit global, remise en conformité et clôture définitive de l'environnement de développement », puis confirmation explicite « Confirmer la dérogation, la reconstruction et les permissions Git » après escalade AGENTS.md §18/§7.

Périmètre : audit Windows/WSL, architecture Linux dev/msm, migration réversible, chemins/lanceurs, outils, authentifications, correction de test déjà autorisée, builds/tests firmware et supervision, gouvernance factuelle, sauvegardes/nettoyage ciblé, documentation et publication vérifiée main. Création d'un journal explicitement reconstruit autorisée si aucun original accessible n'est retrouvé. Aucun contenu historique inventé ni import silencieux.

Autonomie : décisions courantes et corrections d'environnement, commits/push normaux des changements vérifiés dans ce périmètre. Contrôler sources, état Git, diff, secrets, branche et HEAD distant ; aucun force push, merge/rebase automatique ou écrasement de travail utilisateur. La dérogation à l'absence du journal couvre les validations de la mission avant reconstruction.

Interdits : développement fonctionnel TR2, spécifications gelées, mass erase, Option Bytes, flash/debug nouveau sans nécessité justifiée et autorisation conforme, suppression personnelle, formatage, changements de clés/protecteurs BitLocker, affaiblissement de sécurité/Codex. Aucun nouveau matériel nécessaire ici.

Instruction complémentaire utilisateur : aucune nouvelle UAC pour les contrôles administrateur facultatifs ; distinguer les problèmes bloquant réellement le développement des contrôles complémentaires non bloquants. Les lectures administrateur déjà obtenues sont consignées séparément, sans modification Windows de sécurité.

Acceptation : architecture claire avec clone définitif Linux, historiques et correctif préservés, chemins/lanceurs/Codex/GitHub fonctionnels, tests firmware102/102 et .NET466/466 depuis la nouvelle architecture, cross-build ARM, reprise WSL réelle, ST/GDB disponibles, preuves physiques antérieures conservées si artefact inchangé, gouvernance explicite et livrables A–E. Bilan VALIDÉ / VALIDÉ AVEC RÉSERVES / BLOQUÉ. Pas de développement fonctionnel à la clôture.

Ce document consigne le contrat de session ; il ne remplace pas `MISSION_EN_COURS.md`, réservé à ChatGPT. L'ancienne mission et son dernier rapport sont archivés sans altération avant mise à jour du rapport Codex actif. Les permissions expirent à la clôture.
