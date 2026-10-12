---
mission_id: TR2-20261012-M014-H-PRODUCTION-RESUME
status: READY
base_ref: main
base_sha: 30f4663c7eece9ffb66740da1466c0ae88432f5a
created_at_utc: 2026-10-12T07:30:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-H — Reprise ciblée, composition production et qualification physique

## Gouvernance
Lire AGENTS.md, protocole V2-B3, roadmap, état courant, rapport intermédiaire M014-G, manifeste COMMISSIONING_M014_G.json et sources réelles sur main. Vérifier HEAD, Git et deux stashes historiques ; archiver mission et rapport G selon protocole sans écrasement. La mission G est une sauvegarde intermédiaire, pas une qualification. Préserver tous les acquis et preuves hors Git. Ne pas répéter les 113 tests firmware et 539 tests .NET sur code inchangé. Autorisations permanentes confirmées : commit, push, flash, debug STM32, sans demandes de validation répétitives.

## État acquis
UID96 physique 004A003D 38425018 20333157 ; DeviceId1, Modbus unit1, serial TR2-000001, fabricant MSM enregistrés ; diagnostic E et Flash2Mio sauvegardés ; FRAM262144 sauvegardée et doublement vérifiée, image V1 génération10 avec configuration métier non valide ; capteur IIS3DWB WHO_AM_I 0x7B ; microSD détectée, MBR partition type07 à LBA65536 (32Mio) jusqu'à fin de carte. Firmware actuellement flashé : probe isolé lecture seule, pas serveur de production. Aucune identité persistante provisionnée. M014 IN PROGRESS. Consulter rapport G pour SHA, procédures sûres et preuves exactes.

## Travail prioritaire, séquencé
1. Finaliser manifeste opérationnel, registre UID/DeviceId et identité B0 complète selon gels, versions et capacités réellement justifiées. Compléter validate_prefix/configuration initiale et conditions d'autorité sur image V1 génération10 ; ne pas traiter le marqueur 0xAC comme configuration valide. Définir procédure bornée et réversible : config dans slot vide puis migration atomique V1→V2 identité, avec vérification des générations, sauvegardes, readback et arrêt sur divergence. Aucune écriture NVM tant que les préconditions ne sont pas prouvées.
2. Implémenter le strong binding STM32 de production et contextes durables réels : FRAM, capteur, transport RS-485, persistance, états B0/B1 ; microSD optionnelle avec statut honnête si indisponible. Aucun fallback harness, UID tronqué, fausse disponibilité ou faux READY. Ne pas allouer ni écrire dans les 32 premiers Mio de microSD sans propriété exclusive établie ; la partition à partir de 32 Mio est strictement intouchable. Les essais B0/B1 peuvent précéder la qualification bulkSD si conforme aux critères de M014.
3. Construire image production distincte du probe, identifiée et restaurable ; tests ciblés puis tests de non-régression justifiés et cross-build. Flash/debug standards autorisés après preflight cible et procédure de restauration, sans HOTPLUG CubeProgrammer, mass erase, Option Bytes, RDP, reset destructif ni changement de câblage. Exécuter provisioning explicite uniquement avec manifeste et backup fiables, aucune réparation implicite.
4. Vérifier boot, recovery et lecture B0/B1 réels, sans READY artificiel. Préparer outil PC commissioning-check OFFLINE, puis essais SSH HP COM7 bornés et exclusifs 115200 8E1 unit1, logs/hash nouveaux, aucun replay M013-I/M014-C, aucun GDB pendant trafic. Démontrer polling, timeout, invalidation données anciennes et reconnexion physique ; corriger et requalifier si nécessaire.
5. Mettre à jour ETAT_COURANT et ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md avec état M013 exact, statut M014 prouvé et suite M015. Clôturer M014 DONE uniquement si critères physiques satisfaits ; sinon rapport BLOCKED précis avec avancement maximal publié, pas de déclaration de succès fictive.

## Efficacité et sécurité
Ne pas refaire inventaire complet ni campagnes validées sans modification pertinente. Prioriser code production, provisioning et preuves physiques. Commits incrémentaux et push avant épuisement de quota ; rapport et relecture distante. Si interruption/quota, publier point de reprise et préserver stashes. Escalader seulement geste physique nécessaire, opération destructive inhabituelle, risque matériel non maîtrisé ou décision structurante non couverte. Aucun nouvel accord requis pour logiciel, flash/debug standard qualifié et essais bornés. Ne pas modifier le layout V2 approuvé.


## Arbitrage complémentaire ChatGPT du 12/10 — microSD (prioritaire)
Décision d'architecture déléguée par l'utilisateur : pour M014, microSD en lecture seule ou totalement désactivée, **aucune écriture**, aucune attribution ni formatage des 32 premiers Mio, aucune intervention sur la partition existante démarrant LBA65536. Ne pas faire dépendre les essais physiques B0/B1, l'identité persistante FRAM ou le polling supervision de la mise en service bulkSD, sous réserve de vérifier les gels et d'exposer les états réellement indisponibles sans faux READY. Si les normes rendent bulkSD indispensable à un critère précis de clôture M014, consigner exactement ce critère, poursuivre tous les essais indépendants et ne pas prétendre M014 DONE ; ne pas requalifier arbitrairement une exigence normative en option. Traiter l'allocation et la qualification écriture SD dans un lot distinct ultérieur, après démonstration de propriété exclusive et sauvegardes. Cette décision ne requiert aucune approbation utilisateur supplémentaire.

Communication Codex : annoncer chaque tranche engagée, opérations et résultats majeurs, blocages et étape suivante, avec comptes rendus synthétiques mais substantiels ; information non bloquante, sans microvalidations et sans journal verbeux de chaque commande.
