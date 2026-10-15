---
mission_id: TR2-20261015-A06-DECOUPLAGE-SUPERVISION-STOCKAGE-AUDIT
status: READY
base_ref: main
base_sha: 0c88e6ef195b44fdbf21890d946ef7ea8bf190fe
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A-06 — Étude d'impact : supervision vibratoire sans stockage de campagnes

## Décision utilisateur et objectif
Décision structurante du 15 octobre 2026 : découpler entièrement le développement et l'exploitation de la supervision vibratoire de la présence d'une capacité d'enregistrement durable des campagnes. La sauvegarde reste une fonctionnalité industrielle prévue et sera réintégrée lorsque SDMMC ou une autre solution de stockage sera qualifiée. Ne pas abandonner les campagnes, ne pas déclarer la V1 modifiée ou validée. Objectif immédiat : **audit documentaire et de faisabilité**, sans implémentation ni modification des spécifications gelées.

## Périmètre obligatoire
Lire le HEAD réel de main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, état courant, roadmap, journal, mission et dernier rapport CN8, puis parcourir **intégralement et méthodiquement** :
- `Modbus RTU/01_Specification_source/` (blocs 0 à 7, charte, V1_1);
- `Modbus RTU/02_Validation/` (FT et TT pertinents, mapping unifié, qualifications et matrices; distinguer archives historiques, doublons et référentiels actifs).
Compléter par l'architecture, le firmware, les tests et la supervision PC existants uniquement pour vérifier les dépendances réelles et la faisabilité. Reprendre l'obstacle déjà démontré par le rapport `TR2-20261013-M016-V1-VIBRATION`, et les acquis M014-H/M015-V, sans répéter les essais.

## Questions à résoudre
1. Inventorier chaque exigence et test qui suppose la présence d'une microSD ou d'une capacité de stockage : B4 `storage_mode`/`storage_limit_mb`, activation de l'ActiveConfiguration, CRC/mirrors, B5 démarrer/arrêter acquisition, B1 statuts et fautes, B3 calcul/validité/alarmes, B6 inventaire, B7 diagnostics, recovery, persistance, autorité de configuration, supervision PC.
2. Distinguer strictement **acquisition de mesures pour supervision live**, calcul et publication d'indicateurs B3, **enregistrement de campagnes**, et **historisation côté PC SQLite**. Préciser quelles fonctions doivent marcher sans média, lesquelles sont indisponibles et lesquelles peuvent continuer avec une panne/retrait de média, sans fausses données ni perte silencieuse.
3. Proposer la plus petite évolution normative cohérente : maintien des adresses/types/endianness et contrats V1 autant que possible, analyse explicite de la possibilité d'un mode « supervision seule » sans stockage, encodage B4 (y compris CRC et version), compatibilité, migration des configurations persistées, refus anciens, codes d'état et d'erreur, B5, B6 et recovery. Ne jamais traiter `storage_mode=0` comme autorisé dans la V1 gelée sans proposition formelle de changement.
4. Examiner les impacts de `02_Validation` : références FT-STR/ACC/LIM/BLK/INT/SEQ et leurs TT, couverture, oracles, cas nominal/sans média/erreur de média/réintégration, non-régression et preuves physiques nécessaires. Classer chaque élément : inchangé, à compléter, à modifier, nouveau, gel impacté; référencer les chemins exacts.
5. Comparer au moins deux stratégies minimales (extension versionnée du protocole/configuration ou mode interne temporaire strictement borné, éventuellement autre voie justifiée), leurs compromis et recommandation pour progresser vite sans dette de conformité cachée. Établir une trajectoire de migration incrémentale et des critères d'acceptation pour la future mission d'implémentation, puis réintégration du stockage.

## Livrables
- Rapport argumenté `DERNIER_RAPPORT.md` avec matrice de traçabilité exigence → test → code → impact → proposition, priorités, risques et ambiguïtés à arbitrer par ChatGPT.
- Une note d'étude autonome versionnée sous `Modbus RTU/00_gouvernance/` ou chemin approprié, **sans altérer les spécifications, FT/TT gelées, mapping ni code**.
- Préconisation ferme sur faisabilité, compatibilité, ordre de réalisation et distinction des validations documentaires, logicielles et physiques.
- Archiver mission et rapport CN8 existants sans perte, publier par commit/push sur main, puis relire à distance les fichiers et le SHA; rapporter preuves et limites.

## Autonomie et limites
Analyse documentaire/logicielle en lecture seule hors livrables d'audit. Aucun flash, debug, reset, accès ST-LINK, changement de configuration matérielle, recâblage ou écriture FRAM/microSD. Ne pas lancer de nouvelle campagne SDMMC ni demander une décision déjà prise. Ne pas implémenter le nouveau mode ni modifier les gels dans cette mission. Les décisions normatives résiduelles seront présentées à ChatGPT pour arbitrage structurant. Aucun besoin de validation intermédiaire pour mener l'étude. Respecter A-06, préserver les acquis et ne pas substituer une simulation à une preuve physique.
