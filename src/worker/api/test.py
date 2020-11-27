from request import *


def dynamic_binary_test():
    test_db = DynamicBinary(read_only=False)
    test_db.write_template(
        ("Hello", "World", 1, 3, 4),
        "ssiii"
    )

    print(test_db)
    print(test_db.template)


def request_test():
    adr = Address("localhost", 9491)

    test_r = Request(
        adr,
        Request.REQ_SRV_REFRESH,
        (Request.authorize("autogentoo.org", "YAuztP1O3djFfzZZzDDCFy57ran4MtZv"),)
    )

    test_r.send()

    content, code, message = test_r.recv()
    print(content, code, message)


def main(argc, argv):
    # dynamic_binary_test()
    request_test()


if __name__ == '__main__':
    import sys

    main(len(sys.argv), sys.argv)
