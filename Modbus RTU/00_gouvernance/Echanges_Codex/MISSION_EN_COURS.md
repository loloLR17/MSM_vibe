---
mission_id: TR2-20261014-P12H3H-SDMMC1-CABLE10CM-COMPARISON
status: READY
base_ref: main
base_sha: 7271e03e513e3fb5a9ba33051713307125c60bbe
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# P12/H3h — Comparaison ciblée SDMMC1 après remplacement des fils Dupont 20 cm par 10 cm

## Contexte et objectif
Le PoC précédent `TR2-20261014-P12H3H-SDMMC1-REFERENCE-POC` est terminé FAILED : SDMMC1 4 bits 12 MHz démontré, deux jeux de 128 KiB intégralement relus, mais CRC récurrents et USB No Media. Rapport et preuves publiés sur main au HEAD de référence. **Seule modification physique déclarée par l'utilisateur : les fils Dupont reliant directement le breakout Adafruit 4682 aux CN11/CN12 de la seconde Nucleo passent d'environ 20 cm à 10 cm.** Câblage identique, SB19/SB22 pontés et vérifiés antérieurement avec Codex. L'utilisateur confirme Nucleo alimentée, microSD de test 32 Go insérée, USB natif relié au PC à l'identique. Aucun nouveau contrôle physique redondant ni demande de reconnexion par défaut.

Objectif unique : **déterminer si le raccourcissement des fils améliore réellement la fiabilité et, secondairement, les performances de lecture/écriture** à configuration identique. Ne pas confondre comparaison favorable et qualification industrielle/production.

## Périmètre strict et méthode
1. Lire gouvernance courante, dernier rapport et preuves exactes du PoC précédent, vérifier HEAD et état Git ; ne pas réexécuter la mission précédente. Identifier strictement seconde Nucleo ST-LINK `0032001F3235510C37333439`, UID `001B0046/38425018/20333157` et microSD test 32 Go CID `03534453/44333247/858BF092/3C01927A`, capacité `62333952` secteurs 512 octets avant toute écriture. **Ne jamais accéder à la Nucleo principale** ST-LINK `003500463235510B37333439`, à la microSD 512 Go, FRAM ou Modbus principal.
2. Reprendre **la même image Release originale** du firmware communautaire VILLO88/NUCLEOU575 commit `1e3c9183bad023ebf7d234134823d9270a0e8b64`, SHA-256 BIN `2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a`, déjà flashée. Vérifier image/identité et état ; ne reflasher que si nécessaire et justifié, après vérifications de cible/readback. Aucun changement firmware, bus 4 bits, horloge 12 MHz, HAL, middleware, USB, carte ou brochage pendant le comparatif.
3. Procéder à un **test court et discriminant**, en privilégiant la reprise des outils/méthodes exacts du jeu B USB natif : écritures alignées de 16 × 8192 octets = 128 KiB dans une zone de test autorisée et vérifiée, flush, fermeture/réouverture, relecture intégrale, comparaison octet à octet et SHA, mesure des durées d'écriture/lecture, journal des CRC/états carte et éventuel No Media. Conserver les mêmes LBAs de test ou une zone explicitement contrôlée et bornée. Pas de formatage ni écriture hors plage. Si l'état initial est No Media, tenter uniquement la récupération non destructive déjà documentée et consigner le reset nécessaire, sans masquer cette indisponibilité.
4. Pour éviter un faux succès intermittent, si le premier cycle réussit, effectuer **au plus deux cycles courts supplémentaires identiques**, sans changement de configuration ni reset entre cycles tant que tout reste opérationnel. Journaliser chaque cycle, erreurs CRC, disponibilité USB et resets. Si un CRC ou No Media apparaît, **arrêter la répétition**, collecter preuves non destructives et classer la fiabilité comme non améliorée de façon démontrée. Ne pas multiplier les essais ni lancer d'endurance 600 s/7 jours.
5. Comparer explicitement au PoC 20 cm : jeu B 128 KiB write+flush 3178,7446 ms (~41 234 B/s), read 187,4372 ms (~699 285 B/s), mais CRC et No Media récurrents. Les durées sont applicatives hôte/USB, pas débit SD pur ; aucune conclusion causale à partir de petites variations de temps. Si échec précoce, rapporter honnêtement sans essayer de faire passer le résultat au vert.

## Critères de décision
- **VALIDÉ uniquement comme amélioration comparative bornée** si les cycles courts complets (jusqu'à 3) sont exécutés sans CRC, sans No Media, sans reset intermédiaire, sans différence de données, et montrent une disponibilité plus stable que l'essai 20 cm ; comparer les temps sans imposer un gain de débit non significatif. Si un cycle échoue, si le support reste indisponible, ou si preuves insuffisantes : statut FAILED ou INCONCLUSIVE, **ne pas déclarer d'amélioration prouvée**.
- Même si le comparatif est favorable, ne qualifier ni stockage TR2 à 426672 B/s, ni endurance, ni récupération sur erreur, ni adoption du firmware tiers. Ne pas corriger la récupération multi-read/MSC dans cette mission ; diagnostic complémentaire ultérieur séparé si nécessaire.
- Aucun besoin de rejouer les validations firmware globales, campagnes de charge, tests GPIO statiques, tests M017, tests Modbus/FRAM ou intégralité des tests PoC précédents. Éviter rebuild/reflash inutiles.
- En cas d'impossibilité de tester (permissions Windows, périphérique indisponible, etc.), documenter le blocage ; pas de modification des protections système ni opération destructive.

## Autorisations et garde-fous
`allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true` sur `main`, conformément à AGENTS.md. Flash/debug limités à la seconde Nucleo, avec identification de cible, préservation de l'image et de la carte ; aucune mass erase, Option Bytes/RDP, déprotection, formatage, changement de câblage/soudure ou intervention physique par Codex. L'utilisateur conserve les gestes physiques. Préserver les worktrees, stashes, preuves, gels et fichiers préexistants. Aucun correctif opportuniste.

## Livraison
Publier résultats bruts minimaux, méthode reproductible, comparaison 20/10 cm, SHA et état final dans `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` (mission_id ci-dessus), éventuellement dossier de preuves limité. Commit/push sur `main`, puis relecture et vérification distantes. Distinguer `DONE` pour comparaison favorable bornée, `FAILED` pour absence d'amélioration prouvée, `BLOCKED` pour impossibilité d'essai. Ne pas lancer automatiquement la mission suivante. ChatGPT réalisera le contrôle indépendant.
